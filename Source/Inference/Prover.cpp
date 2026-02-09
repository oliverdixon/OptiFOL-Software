/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the FOL Resolution Knowledge Base
 * @author Oliver Dixon
 * @date 2025-09-15
 * @version Development
 */

#include "Prover.hpp"

#include <algorithm>
#include <queue>
#include <ranges>
#include <sigc++/adaptors/bind.h>

#include "../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../IR/Sentences/Literal.hpp"
#include "../IR/SymbolRepository.hpp"
#include "../Logging.hpp"
#include "ExpressionFactory.hpp"
#include "QueryResult.hpp"
#include "Resolvent.hpp"

namespace optifol
{

const log4cxx::LoggerPtr Prover::prover_logger = Logging::get_logger({"LogicServices", "Prover"});
const log4cxx::LoggerPtr Prover::resolution_logger = Logging::get_logger({"LogicServices", "Prover", "Resolution"});
const log4cxx::LoggerPtr Prover::factoring_logger = Logging::get_logger({"LogicServices", "Prover", "Factoring"});

Prover::Prover(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository)),
    base_clauses(this->symbol_repository),
    unifier(this->symbol_repository),
    factoring_unifier(this->symbol_repository),
    applicator(unifier.share_substitutions(), this->symbol_repository),
    factoring_applicator(factoring_unifier.share_substitutions(), this->symbol_repository)
{
}

bool Prover::tell(const SentenceRoot &sentence)
{
    return std::ranges::all_of(sentence, [this](const Clause& new_clause)
    {
        return tell(new_clause);
    });
}

bool Prover::tell(const Clause &new_clause)
{
    return base_clauses.add_clause(std::make_unique<Clause>(new_clause)).second;
}

bool Prover::PQResolventUnitPref::operator()(
        const std::unique_ptr<Resolvent> &lhs, const std::unique_ptr<Resolvent> &rhs) noexcept
{
    return *lhs < *rhs;
}

QueryResult Prover::run_resolution(std::unique_ptr<SentenceRoot> &&negated_query, const size_t max_step_count)
{
    QueryResult result(base_clauses); // Execution trace, introduced clauses, and metadata built up by the solver.

    // Introduce the negated goal clauses into the query instance.
    for (const auto& clause : *negated_query)
        result.introduced_clauses.add_clause(std::make_unique<Clause>(clause));

    /*
     * Over the Cartesian product of clauses in the KB, search for resolvents in the initial set and populate the
     * working queue accordingly. Note that binary resolution is a commutative operation, so we restrict the RHS.
     */

    ResolventQueue working_queue; // Generated resolvents not yet committed to introduced knowledge.
    for (const auto lhs_clause : result.introduced_clauses.flatten())
        for (const auto& rhs_clause : result.introduced_clauses.flatten()) {
            auto new_resolvents = find_resolvents(lhs_clause, rhs_clause);
            for (auto&& [resolvent, owning_resolution] : new_resolvents)
                working_queue.push(std::move(resolvent), std::move(owning_resolution));
        }

    resolution_logger->debug(std::format("Resolution initial search gathered {} resolvents.", working_queue.size()));
    RawUnorderedSet<const Clause> seen_resolvents; // Transparent hashing to provide lightweight de-duplication.

    /*
     * Once the initial set of resolvents has been added to the priority queue, continue stepping until a result has
     * been produced to indicate whether the negated query induces an inconsistent KB.
     */
    for (; !working_queue.empty(); ++result.elapsed_step_count) {
        auto next_resolvent = working_queue.pop();

        /*
         * Mark the resolved clause as "seen". If it is being seen for the first time, the popped resolvent is committed
         * to the final execution trace, and ownership of the resolved clause is transferred likewise. If it has been
         * seen before, we can just ignore it, as binary resolution can repeat resolutions for different clauses.
         */
        auto [_, was_unseen] = seen_resolvents.insert(next_resolvent.observe_node());

        if (was_unseen) {
            resolution_logger->info(std::format("Step {} is using resolution {}.", result.elapsed_step_count,
                *next_resolvent.observe_node()));

            if (result.introduced_clauses.is_orphaned(next_resolvent.observe_node()))
                // Keep the (non-owning) resolvent popped off the working queue, but don't move its resolution.
                continue;

            /*
             * Insert the next-unseen resolvent into the KB by transferring ownership of the corresponding resolution
             * into the QueryResult and recording the resolvent into the trace. If the resolution clause is non-trivial
             * and new, search for more resolvents.
             *
             * If the KB refuses ownership of the new Clause (i.e. if it would be subsumed by the existing KB), we take
             * ownership back and return it to the working queue. Otherwise, it would go out of scope here despite other
             * resolvents potentially still using it.
             */
            auto owning_resolution = working_queue.extract_resolution(next_resolvent);

            auto [committed_resolution_it, was_committed] =
                result.introduced_clauses.add_clause(std::move(owning_resolution),
                    sigc::mem_fun(working_queue, &ResolventQueue::store_resolution));

            result.relations.push_back(std::move(next_resolvent));

            if (was_committed) {
                if ((*committed_resolution_it)->get_triviality_state() == Clause::State::TriviallyFalse) {
                    // An empty derived clause indicates that a contradiction was derived in the KB.
                    result.outcome = QueryResult::ConjectureStatus::Consistent;
                    result.terminating_resolvent = &result.relations.back();
                    working_queue.dump(result.relations, result.introduced_clauses);
                    return result;
                }

                const Resolvent * const lhs = &result.relations.back();

                // Clauses on RHS
                for (const auto rhs_clause : base_clauses.flatten()) {
                    auto new_resolvents = find_resolvents(lhs, rhs_clause);
                    for (auto&& [new_resolvent, new_resolution] : new_resolvents)
                        working_queue.push(std::move(new_resolvent), std::move(new_resolution));
                }

                // Resolvents on RHS
                for (const auto& rhs_resolvent : result.relations) {
                    auto new_resolvents = find_resolvents(lhs, &rhs_resolvent);
                    for (auto&& [new_resolvent, new_resolution] : new_resolvents)
                        working_queue.push(std::move(new_resolvent), std::move(new_resolution));
                }
            }
        } else
            resolution_logger->debug(std::format("Resolution skipping step {} due to repeated resolvent.",
                result.elapsed_step_count));

        if (result.elapsed_step_count == max_step_count) {
            result.outcome = QueryResult::ConjectureStatus::TimedOut;
            working_queue.dump(result.relations, result.introduced_clauses);
            return result;
        }
    }

    /*
     * If we have exhausted all resolvents in the PQ without finding a contradiction, then introduced of the negated
     * query did not induce an inconsistent KB.
     */
    result.outcome = QueryResult::ConjectureStatus::Inconsistent;
    working_queue.dump(result.relations, result.introduced_clauses);
    return result;
}

QueryResult Prover::ask(std::unique_ptr<MutableSentenceRoot> &&query, const size_t max_step_count)
{
    // Produce the negation of the goal.
    query->flip_polarity();
    auto negated_query = ExpressionFactory::build_sentence(std::move(query), symbol_repository);

    const auto clause_count = base_clauses.get_total_clause_count();
    prover_logger->info(std::format("Querying the KB of {} clauses for the negation of {}.", clause_count, *negated_query));

    if (prover_logger->isTraceEnabled()) {
        prover_logger->trace("Dumping initial knowledge base...");
        unsigned int clause_idx = 1;
        for (const auto clause : base_clauses.flatten())
            prover_logger->trace(std::format("KB Clause {}/{}: {}", clause_idx++, clause_count, *clause));

        prover_logger->trace(std::format("KB Clause (negated goal): {}", *negated_query->begin()));
    }

    // Run the theorem-prover.
    QueryResult result = run_resolution(std::move(negated_query), max_step_count);

    switch (result.outcome) {
    case QueryResult::ConjectureStatus::TimedOut:
        prover_logger->info(std::format("The solver timed out after {} steps.", max_step_count));
        break;
    case QueryResult::ConjectureStatus::Consistent:
        prover_logger->info(std::format("A contradiction was derived in {} steps; the queried sentence is consistent.",
            result.elapsed_step_count));
        break;
    case QueryResult::ConjectureStatus::Inconsistent:
        prover_logger->info(std::format("A contradiction could not be derived after {} steps, and no more resolutions were "
                                    "available. The queried sentence is inconsistent.", result.elapsed_step_count));
        break;
    case QueryResult::ConjectureStatus::NotExecuted:
        prover_logger->error("The solver panicked, and no proof was attempted.");
        break;
    }

    return result;
}

std::unique_ptr<Clause> Prover::collect_unified_literals(
        const Literal &self, const Clause &source_clause, std::unique_ptr<Clause> &&destination_clause) const
{
    const auto transformer =
        std::views::filter([self](const Literal * candidate) { return !self.operator==(*candidate); }) |
        std::views::transform([this](const Literal * target) { return target->accept(applicator); });

    for (const auto transformed_literal : source_clause | transformer)
        destination_clause->add_literal(transformed_literal);

    return destination_clause;
}

std::unique_ptr<Clause> Prover::factor_literals(std::unique_ptr<Clause> &&unified_clause)
{
    bool factoring_done = false;
    bool trivially_true = false;
    auto working_clause = std::make_unique<Clause>(*unified_clause);

    do {
        factoring_done = false;

        for (const auto [factoring_idx, factoring_lhs_literal] : *unified_clause | std::views::enumerate) {
            for (const auto factoring_rhs_literal : *unified_clause | std::views::take(factoring_idx)) {
                if (working_clause != nullptr && working_clause->get_triviality_state() ==
                        Clause::State::TriviallyTrue) {
                    trivially_true = true;
                    break;
                }

                /*
                 * Finding a unifying MGU between the factoring literals indicates an opportunity to reduce the clause
                 * size. Once the MGU is obtained, we can replace the working clause with the simplified variant, under
                 * application of the MGU.
                 */
                if (factoring_lhs_literal->accept(factoring_unifier, *factoring_rhs_literal)) {
                    factoring_logger->info(std::format("Successfully unified {} and {}.", *factoring_lhs_literal,
                        *factoring_rhs_literal));

                    auto simplified = std::make_unique<Clause>();
                    std::ranges::for_each(*unified_clause,
                        [this, &simplified](const Literal * literal)
                        {
                            simplified->add_literal(literal->accept(factoring_applicator));
                        }
                    );

                    factoring_logger->info(std::format("Factoring produced a new working clause of order {}.",
                        simplified->order()));

                    working_clause.swap(simplified);
                    factoring_applicator.keep_new_symbols();
                    factoring_done = true;
                    break;
                }
            }

            if (!trivially_true || factoring_done)
                break;
        }
    } while (!trivially_true && factoring_done);

    return working_clause;
}

bool Prover::insert_clause(std::unique_ptr<Clause> &&new_clause, UniqueUnorderedSet<Clause> &destination)
{
    const auto [node_it, added_ok] = destination.emplace(std::move(new_clause));
    std::ignore = node_it;

    if (!added_ok) {
        prover_logger->info("Rejecting clause from the KB as it is already present.");
        return false;
    }

    return true;
}

std::vector<std::pair<Resolvent, std::unique_ptr<Clause>>> Prover::find_resolvents(
        const ProofTreeNode *const lhs_node, const ProofTreeNode *const rhs_node)
{
    std::vector<std::pair<Resolvent, std::unique_ptr<Clause>>> resolvents;

    const auto * const lhs_clause = lhs_node->observe_node();
    const auto * const rhs_clause = rhs_node->observe_node();

    for (const auto lhs_literal : *lhs_clause)
        for (const auto rhs_literal : *rhs_clause) {

            // Attempt to unify the LHS literal with the negation of the RHS literal.
            auto rhs_args_copy = rhs_literal->observe_arguments();
            const Literal negated_rhs(std::string(rhs_literal->get_name()), std::move(rhs_args_copy),
                rhs_literal->is_negative_polarity());

            if (lhs_literal->accept(unifier, negated_rhs)) {

                /*
                 * If the LHS and negated RHS can be unified, we have attained a set of most-general unifiers. Construct
                 * the final resolvents according to the predicate resolution rule: apply the unifier to the union of
                 * all literals except the pair removed by unification. For the LHS and RHS clauses, over all literals
                 * except the pair removed by resolution, apply the MGU and add to the resolvent set.
                 *
                 * Note that application of the MGU might create new literals. In that case, they are added to the
                 * general symbol store so we only have to see non-owning, raw, immutable pointers.
                 */

                auto resolution = std::make_unique<Clause>();
                resolution = collect_unified_literals(*lhs_literal, *lhs_clause, std::move(resolution));
                resolution = collect_unified_literals(*rhs_literal, *rhs_clause, std::move(resolution));

                resolution_logger->debug(std::format("Constructed a unified clause of {} literals.",
                    resolution->order()));

                /*
                 * Attempt to simplify the unified clause, and guarantee a complete inference process, by:
                 *
                 *  1. Eliminating entire clauses that are trivially true, containing self-negating literals; and
                 *  2. Eliminating literals that can be unified.
                 *
                 * Once the clause has been built according to the above exclusion criteria, it is added to the
                 * resolvent vector returned to the caller.
                 *
                 * Note that (1) doesn't need explicit handling here, as the clause will reduce to TriviallyTrue if
                 * a pair of complementary literals are added.
                 */

                if (resolution->get_triviality_state() == Clause::State::TriviallyTrue) {
                    resolution_logger->debug("Before factoring, the unified clause is a tautology; continuing.");
                    applicator.discard_new_symbols();
                    continue;
                }

                resolution = factor_literals(std::move(resolution));

                if (resolution->get_triviality_state() == Clause::State::TriviallyTrue) {
                    resolution_logger->debug("After factoring, the unified clause is a tautology; continuing.");
                    applicator.discard_new_symbols();
                    continue;
                }

                /*
                 * A non-trivial clause produced through resolution should be considered a fresh resolvent. This is
                 * described by the source LHS and RHS clauses, the MGU, and the unified clause of the post-
                 * application literals as a clause under disjunction.
                 */

                resolvents.emplace_back(Resolvent(lhs_node, rhs_node, *unifier.observe_substitutions(),
                    resolution.get()), std::move(resolution));

                resolution_logger->debug(std::format("Resolved {} and {} to {}.", *lhs_clause, *rhs_clause,
                    *resolvents.back().first.observe_node()));

                // The applicator might have introduced new symbols, so we inherit them into the SymbolRepository here.
                applicator.keep_new_symbols();
            }

            // Any important MGUs have been copied into a resolvent.
            unifier.reset_substitutions();
        }

    return resolvents;
}

} // namespace optifol
