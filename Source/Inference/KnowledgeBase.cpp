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

#include "KnowledgeBase.hpp"

#include <algorithm>
#include <queue>
#include <ranges>

#include "../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../IR/Sentences/Literal.hpp"
#include "../Logging.hpp"
#include "ExpressionFactory.hpp"
#include "QueryResult.hpp"
#include "Resolvent.hpp"

namespace optifol
{

const log4cxx::LoggerPtr KnowledgeBase::kb_logger = Logging::get_logger({"LogicServices", "KnowledgeBase"});
const log4cxx::LoggerPtr KnowledgeBase::resolution_logger = Logging::get_logger({"LogicServices", "KnowledgeBase",
    "Resolution"});
const log4cxx::LoggerPtr KnowledgeBase::factoring_logger = Logging::get_logger({"LogicServices", "KnowledgeBase",
    "Factoring"});

KnowledgeBase::KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository)),
    unifier(this->symbol_repository),
    factoring_unifier(this->symbol_repository),
    applicator(unifier.share_substitutions(), this->symbol_repository),
    factoring_applicator(factoring_unifier.share_substitutions(), this->symbol_repository)
{
}

bool KnowledgeBase::tell(const SentenceRoot &sentence)
{
    base_clauses.reserve(base_clauses.size() + sentence.order());
    return std::ranges::all_of(sentence, [this](const Clause& new_clause)
    {
        return insert_clause(std::make_unique<Clause>(new_clause), base_clauses);
    });
}

bool KnowledgeBase::tell(const Clause &new_clause)
{
    return insert_clause(std::make_unique<Clause>(new_clause), base_clauses);
}

bool KnowledgeBase::PQResolventUnitPref::operator()(
        const std::unique_ptr<Resolvent> &lhs, const std::unique_ptr<Resolvent> &rhs) noexcept
{
    return *lhs < *rhs;
}

QueryResult KnowledgeBase::run_resolution(std::unique_ptr<SentenceRoot> &&negated_query, const size_t max_step_count)
{
    RawUnorderedSet<const Clause> seen_resolvents; // Transparent hashing to provide lightweight de-duplication.
    ResolventQueue working_queue; // Generated resolvents not yet committed to introduced knowledge.
    QueryResult result; // Execution trace, introduced clauses, and metadata built up by the solver.

    // Introduce the negated goal clauses into the query instance.
    result.introduced_clauses.reserve(result.introduced_clauses.size() + negated_query->order());
    for (const auto& clause : *negated_query)
        insert_clause(std::make_unique<Clause>(clause), result.introduced_clauses);

    /*
     * Over the Cartesian product of clauses in the KB, search for resolvents in the initial set and populate the
     * working queue accordingly. Note that binary resolution is a commutative operation, so we restrict the RHS.
     */
    const auto initial_clauses = std::ranges::concat_view(base_clauses, result.introduced_clauses) | unwrap_clause;
    for (const auto [clause_idx, lhs_clause] : initial_clauses | std::views::enumerate)
        for (const auto& rhs_clause : initial_clauses | std::views::take(clause_idx + 1)) {
            auto new_resolvents = find_resolvents(lhs_clause, rhs_clause);
            for (auto&& [resolvent, owning_resolution] : new_resolvents)
                working_queue.push(std::move(resolvent), std::move(owning_resolution));
        }

    resolution_logger->debug(std::format("Resolution initial search gathered {} resolvents.", working_queue.size()));

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
        auto [seen_resolution_it, was_unseen] = seen_resolvents.insert(
            next_resolvent.observe_substance());
        std::ignore = seen_resolution_it;

        if (was_unseen) {
            resolution_logger->info(std::format("Step {} is using resolution {}.", result.elapsed_step_count,
                *next_resolvent.observe_substance()));

            /*
             * Insert the next-unseen resolvent into the KB by transferring ownership of the corresponding resolution
             * into the QueryResult and recording the resolvent into the trace. If the resolution clause is non-trivial
             * and new, search for more resolvents.
             */
            auto owning_resolution = working_queue.extract_resolution(next_resolvent);
            auto [committed_resolution_it, was_committed] =
                result.introduced_clauses.insert(std::move(owning_resolution));
            result.relations.push_back(std::move(next_resolvent));

            if ((*committed_resolution_it)->get_triviality_state() == Clause::State::TriviallyFalse) {
                // An empty derived clause indicates that a contradiction was derived in the KB.
                result.outcome = QueryResult::ConjectureStatus::Consistent;
                result.terminating_resolvent = &result.relations.back();
                working_queue.dump(result.relations, result.introduced_clauses);
                return result;
            }

            if (was_committed) {
                const Resolvent * const lhs = &result.relations.back();

                // Clauses on RHS
                for (const auto& rhs_clause : base_clauses | unwrap_clause) {
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

QueryResult KnowledgeBase::ask(std::unique_ptr<MutableSentenceRoot> &&query, const size_t max_step_count)
{
    // Produce the negation of the goal.
    query->flip_polarity();
    auto negated_query = ExpressionFactory::build_sentence(std::move(query), symbol_repository);

    kb_logger->info(std::format("Querying the KB of {} clauses for the negation of {}.", base_clauses.size(),
        *negated_query));

    if (kb_logger->isTraceEnabled()) {
        kb_logger->trace("Dumping initial knowledge base...");
        const auto clause_count = base_clauses.size();
        for (const auto& [clause_idx, clause]: base_clauses | unwrap_clause | std::views::enumerate)
            kb_logger->trace(std::format("KB Clause {}/{}: {}", clause_idx + 1, clause_count, *clause));

        kb_logger->trace(std::format("KB Clause (negated goal): {}", *negated_query->begin()));
    }

    // Run the theorem-prover.
    QueryResult result = run_resolution(std::move(negated_query), max_step_count);

    switch (result.outcome) {
    case QueryResult::ConjectureStatus::TimedOut:
        kb_logger->info(std::format("The solver timed out after {} steps.", max_step_count));
        break;
    case QueryResult::ConjectureStatus::Consistent:
        kb_logger->info(std::format("A contradiction was derived in {} steps; the queried sentence is consistent.",
            result.elapsed_step_count));
        break;
    case QueryResult::ConjectureStatus::Inconsistent:
        kb_logger->info(std::format("A contradiction could not be derived after {} steps, and no more resolutions were "
                                    "available. The queried sentence is inconsistent.", result.elapsed_step_count));
        break;
    case QueryResult::ConjectureStatus::NotExecuted:
        kb_logger->error("The solver panicked, and no proof was attempted.");
        break;
    }

    return result;
}

std::unique_ptr<Clause> KnowledgeBase::collect_unified_literals(
        const Literal &self, const Clause &source_clause, std::unique_ptr<Clause> &&destination_clause) const
{
    const auto transformer =
        std::views::filter([self](const Literal * candidate) { return !self.operator==(*candidate); }) |
        std::views::transform([this](const Literal * target) { return target->accept(applicator); });

    for (const auto transformed_literal : source_clause | transformer)
        destination_clause->add_literal(transformed_literal);

    return destination_clause;
}

std::unique_ptr<Clause> KnowledgeBase::factor_literals(std::unique_ptr<Clause> &&unified_clause)
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

bool KnowledgeBase::insert_clause(std::unique_ptr<Clause> &&new_clause, UniqueUnorderedSet<Clause> &destination)
{
    const auto [node_it, added_ok] = destination.emplace(std::move(new_clause));
    std::ignore = node_it;

    if (!added_ok) {
        kb_logger->info("Rejecting clause from the KB as it is already present.");
        return false;
    }

    return true;
}

std::vector<std::pair<Resolvent, std::unique_ptr<Clause>>> KnowledgeBase::find_resolvents(
        const ProofTreeNode *const lhs_node, const ProofTreeNode *const rhs_node)
{
    std::vector<std::pair<Resolvent, std::unique_ptr<Clause>>> resolvents;

    const auto * const lhs_clause = lhs_node->observe_substance();
    const auto * const rhs_clause = rhs_node->observe_substance();

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
                    *resolvents.back().first.observe_substance()));

                // The applicator might have introduced new symbols, so we inherit them into the SymbolRepository here.
                applicator.keep_new_symbols();
            }

            // Any important MGUs have been copied into a resolvent.
            unifier.reset_substitutions();
        }

    return resolvents;
}

} // namespace optifol
