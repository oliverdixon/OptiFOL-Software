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

#include "../Logging.hpp"
#include "../IR/Sentences/Literal.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationApplicationVisitor.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

const log4cxx::LoggerPtr KnowledgeBase::kb_logger = Logging::get_logger({"LogicServices", "KnowledgeBase"});
const log4cxx::LoggerPtr KnowledgeBase::resolution_logger = Logging::get_logger({"LogicServices", "KnowledgeBase",
    "Resolution"});
const log4cxx::LoggerPtr KnowledgeBase::factoring_logger = Logging::get_logger({"LogicServices", "KnowledgeBase",
    "Factoring"});

KnowledgeBase::KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository))
{
}

void KnowledgeBase::tell(const SentenceRoot &sentence)
{
    clauses.reserve(clauses.size() + sentence.order());
    for (const auto& clause : sentence)
        tell(clause);
}

void KnowledgeBase::tell(const Clause &clause)
{
    clauses.push_back(clause);
}

bool KnowledgeBase::query(const SentenceRoot &negated_query)
{
    kb_logger->info(std::format("Querying the KB of {} clauses for the negation of {}.", clauses.size(),
        negated_query));

    // Add the negated query to the KB in an attempt to derive a contradiction.
    tell(negated_query);

    if (kb_logger->isTraceEnabled()) {
        const auto clause_count = clauses.size();
        for (const auto& [clause_idx, clause]: std::views::enumerate(clauses))
            kb_logger->trace(std::format("KB Clause {}/{}: {}", clause_idx + 1, clause_count, clause));
    }

    std::priority_queue<Resolvent> resolvents;

    // Over the Cartesian product of clauses in the KB, add pairwise resolvents.
    for (const auto& lhs_clause : clauses)
        for (const auto& rhs_clause : clauses) {
            auto new_resolvents = find_resolvents(lhs_clause, rhs_clause);
            for (auto new_resolvent : new_resolvents)
                resolvents.push(std::move(new_resolvent));
        }

    kb_logger->debug(std::format("Completed initial search; gathered {} resolvents.", resolvents.size()));

    /*
     * Once the initial set of resolvents has been added to the priority queue, continue stepping until a result has
     * been produced to indicate whether the negated query induces an inconsistent KB.
     */

    unsigned int step_num = 1;
    for (; !resolvents.empty(); ++step_num) {
        const auto resolution = resolvents.top().observe_resolution();
        resolvents.pop();

        kb_logger->debug(std::format("Step {} is using resolution {}.", step_num, resolution));

        if (resolution.get_triviality_state() == Clause::State::TriviallyFalse) {
            // An empty unified clause indicates that a contradiction was derived in the KB.
            kb_logger->info(std::format("Step {} derived a contradiction; the queried sentence is true.", step_num));
            return true;
        }

        /*
         * If no contradiction was derived for this resolvent, insert it into the KB. If it is something new, attempt to
         * find new resolvents.
         */
        tell(resolution); // TODO doesn't restrict to new clauses because the store is a vector...
        for (const auto& rhs_clause : clauses) {
            auto new_resolvents = find_resolvents(resolution, rhs_clause);
            for (auto new_resolvent : new_resolvents)
                resolvents.push(std::move(new_resolvent));
        }
    }

    /*
     * If we have exhausted all resolvents in the PQ without finding a contradiction, then introduced of the negated
     * query did not induce an inconsistent KB.
     */
    kb_logger->info(std::format("Did not derive a contradiction after {} steps; the queried sentence is false.",
        step_num));

    return false;
}

void KnowledgeBase::collect_unified_literals(const Literal &self, const Clause &source_clause,
        Clause &destination_clause, UnificationApplicationVisitor &applicator)
{
    auto unified = source_clause |
        std::views::filter([self](const Literal * candidate) { return !self.operator==(*candidate); }) |
        std::views::transform([&applicator](const Literal * target) { return target->accept(applicator); });

    std::ranges::for_each(unified,
        [&destination_clause](const Literal * literal) { destination_clause.add_literal(literal); } );
}

Clause KnowledgeBase::factor_literals(const Clause &unified_clause, UnificationVisitor &unifier,
    UnificationApplicationVisitor &applicator)
{
    bool factoring_done = false;
    bool trivially_true = false;
    Clause working_clause = unified_clause;

    do {
        factoring_done = false;

        for (const auto [factoring_idx, factoring_lhs_literal] : std::views::enumerate(unified_clause)) {
            for (const auto factoring_rhs_literal : unified_clause | std::views::take(factoring_idx)) {
                if (working_clause.get_triviality_state() == Clause::State::TriviallyTrue) {
                    trivially_true = true;
                    break;
                }

                /*
                 * Finding a unifying MGU between the factoring literals indicates an opportunity to reduce the clause
                 * size. Once the MGU is obtained, we can replace the working clause with the simplified variant, under
                 * application of the MGU.
                 */
                if (factoring_lhs_literal->accept(unifier, *factoring_rhs_literal)) {
                    factoring_logger->info(std::format("Successfully unified {} and {}.", *factoring_lhs_literal,
                        *factoring_rhs_literal));

                    Clause simplified;
                    std::ranges::for_each(unified_clause,
                        [&simplified, &applicator](const Literal * literal)
                        {
                            simplified.add_literal(literal->accept(applicator));
                        }
                    );

                    factoring_logger->info(std::format("Reduced the working clause size to {} from {}.",
                        simplified.order(), working_clause.order()));

                    working_clause = std::move(simplified);
                    applicator.keep_new_symbols();
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

std::vector<Resolvent> KnowledgeBase::find_resolvents(const Clause &lhs_clause, const Clause &rhs_clause) const
{
    UnificationVisitor unifier(symbol_repository);
    UnificationApplicationVisitor applicator(unifier.observe_substitutions(), symbol_repository);

    UnificationVisitor factoring_unifier(symbol_repository);
    UnificationApplicationVisitor factoring_applicator(unifier.observe_substitutions(), symbol_repository);

    std::vector<Resolvent> resolvents;

    for (const auto lhs_literal : lhs_clause)
        for (const auto rhs_literal : rhs_clause) {

            // Attempt to unify the LHS literal with the negation of the RHS literal.
            auto rhs_args_copy = rhs_literal->observe_arguments();
            const Literal negated_rhs(std::string(rhs_literal->get_name()), std::move(rhs_args_copy),
                rhs_literal->is_negative_polarity());

            if (lhs_literal->accept(unifier, negated_rhs)) {

                resolution_logger->info(std::format("Successfully unified {} and {}.", *lhs_literal, negated_rhs));

                /*
                 * If the LHS and negated RHS can be unified, we have attained a set of most-general unifiers. Construct
                 * the final resolvents according to the predicate resolution rule: apply the unifier to the union of
                 * all literals except the pair removed by unification. For the LHS and RHS clauses, over all literals
                 * except the pair removed by resolution, apply the MGU and add to the resolvent set.
                 *
                 * Note that application of the MGU might create new literals. In that case, they are added to the
                 * general symbol store so we only have to see non-owning, raw, immutable pointers.
                 */

                Clause resolution;
                collect_unified_literals(*lhs_literal, lhs_clause, resolution, applicator);
                collect_unified_literals(*rhs_literal, rhs_clause, resolution, applicator);

                resolution_logger->debug(std::format("Constructed a unified clause of {} literals.",
                    resolution.order()));

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

                if (resolution.get_triviality_state() == Clause::State::TriviallyTrue) {
                    resolution_logger->debug("Before factoring, the unified clause is a tautology; continuing.");
                    applicator.discard_new_symbols();
                    continue;
                }

                resolution = factor_literals(resolution, factoring_unifier, factoring_applicator);

                if (resolution.get_triviality_state() == Clause::State::TriviallyTrue) {
                    resolution_logger->debug("After factoring, the unified clause is a tautology; continuing.");
                    applicator.discard_new_symbols();
                    continue;
                }

                /*
                 * A non-trivial clause produced through resolution should be considered a fresh resolvent. This is
                 * described by the source LHS and RHS clauses, the MGU, and the unified clause of the post-
                 * application literals as a clause under disjunction.
                 */
                resolvents.emplace_back(lhs_clause, rhs_clause, unifier.observe_substitutions(),
                    std::move(resolution));

                resolution_logger->info(std::format("Resolved {} and {} to {}.", lhs_clause, rhs_clause,
                    resolvents.back().observe_resolution()));

                // The applicator might have introduced new symbols, so we inherit them into the SymbolRepository here.
                applicator.keep_new_symbols();
            }

            // Any important MGUs have been copied into a resolvent.
            unifier.reset_substitutions();
        }

    return resolvents;
}

} // namespace optifol
