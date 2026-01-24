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

#include <queue>

#include "../IR/Sentences/Literal.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationApplicationVisitor.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

KnowledgeBase::KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository))
{
}

void KnowledgeBase::tell(const SentenceRoot &sentence)
{
    clauses.reserve(clauses.size() + sentence.get_clause_count());
    for (const auto& clause : sentence)
        tell(clause);
}

void KnowledgeBase::tell(const SentenceRoot::Clause &clause)
{
    clauses.push_back(clause);
}

bool KnowledgeBase::query_negative(const SentenceRoot &negated_query)
{
    std::priority_queue<Resolvent> resolvents;

    // Add the negated query to the KB in an attempt to derive a contradiction.
    tell(negated_query);

    // Over the Cartesian product of clauses in the KB, add pairwise resolutions to the priority queue.
    for (const auto& lhs_clause : clauses)
        for (const auto& rhs_clause : clauses) {
            auto new_resolvents = find_resolvents(lhs_clause, rhs_clause);
            for (auto new_resolvent : new_resolvents)
                resolvents.push(std::move(new_resolvent));
        }

    /*
     * Once the initial set of resolvents has been added to the priority queue, continue stepping until a result has
     * been produced to indicate whether the negated query induces an inconsistent KB.
     */

    while (!resolvents.empty()) {
        const auto resolvent = resolvents.top();
        resolvents.pop();

        if (resolvent.unified_clause.empty())
            // An empty unified clause indicates that a contradiction was derived in the KB.
            return true;

        /*
         * If no contradiction was derived for this resolvent, insert it into the KB. If it is something new, attempt to
         * find new resolvents.
         */
        tell(resolvent.unified_clause); // TODO doesn't restrict to new clauses because the store is a vector...
        for (const auto& rhs_clause : clauses) {
            auto new_resolvents = find_resolvents(resolvent.unified_clause, rhs_clause);
            for (auto new_resolvent : new_resolvents)
                resolvents.push(std::move(new_resolvent));
        }
    }

    /*
     * If we have exhausted all resolvents in the PQ without finding a contradiction, then introduced of the negated
     * query did not induce an inconsistent KB.
     */
    return false;
}

std::vector<KnowledgeBase::Resolvent> KnowledgeBase::find_resolvents(
        const SentenceRoot::Clause &lhs_clause, const SentenceRoot::Clause &rhs_clause) const
{
    UnificationVisitor unification_visitor(symbol_repository);
    UnificationVisitor factoring_unification_visitor(symbol_repository);

    UnificationApplicationVisitor applicator(unification_visitor.observe_substitutions(), symbol_repository);
    UnificationApplicationVisitor factoring_applicator(unification_visitor.observe_substitutions(), symbol_repository);

    std::vector<Resolvent> resolvents;

    for (const auto lhs_literal : lhs_clause)
        for (const auto rhs_literal : rhs_clause) {

            // Attempt to unify the LHS literal with the negation of the RHS literal.

            auto rhs_args_copy = rhs_literal->observe_arguments();
            const Literal negated_rhs(std::string(rhs_literal->get_name()), std::move(rhs_args_copy),
                rhs_literal->is_negative_polarity());

            if (lhs_literal->accept(unification_visitor, negated_rhs)) {

                /*
                 * If the LHS and negated RHS can be unified, we have attained a set of most-general unifiers. Construct
                 * the final resolvents according to the predicate resolution rule: apply the unifier to the union of
                 * all literals except the pair removed by unification. For the LHS and RHS clauses, over all literals
                 * except the pair removed by resolution, apply the MGU and add to the resolvent set.
                 *
                 * Note that application of the MGU might create new literals. In that case, they are added to the
                 * general symbol store so we only have to see non-owning, raw, immutable pointers.
                 */

                RawUnorderedSet<const Literal> unified_literals;

                for (const auto lhs_other : lhs_clause)
                    if (lhs_other != lhs_literal)
                        unified_literals.insert(lhs_other->accept(applicator));

                for (const auto rhs_other : rhs_clause)
                    if (rhs_other != rhs_literal)
                        unified_literals.insert(rhs_other->accept(applicator));

                /*
                 * Attempt to simplify the unified clause, and guarantee a complete inference process, by:
                 *
                 *  1. Eliminating entire clauses that are trivially true, containing self-negating literals; and
                 *  2. Eliminating literals that can be unified.
                 *
                 * Once the clause has been built according to the above exclusion criteria, it is added to the
                 * resolvent vector returned to the caller.
                 */

                bool trivial = false;
                bool factored = false;

                do {
                    factored = false;

                    for (const auto factoring_lhs_literal : unified_literals) {
                        // TODO don't need to run over all literals here.
                        for (const auto factoring_rhs_literal : unified_literals) {
                            if (factoring_lhs_literal == factoring_rhs_literal)
                                continue;

                            /*
                             * Any clauses that would be trivially true, where a literal is a negation of itself, can be
                             * discarded, as it would never produce a useful resolvent.
                             */
                            if (factoring_lhs_literal->equals_negation(*factoring_rhs_literal)) {
                                trivial = true;
                                break;
                            }

                            /*
                             * Finding a unifying MGU between the factoring literals indicates an opportunity to reduce
                             * the clause size. Once the MGU is obtained, we can replace the working clause with the
                             * simplified variant, under application of the MGU.
                             */
                            if (factoring_lhs_literal->accept(factoring_unification_visitor,
                                    *factoring_rhs_literal)) {
                                RawUnorderedSet<const Literal> simplified_unified_literals;

                                for (const auto target_literal : unified_literals)
                                    simplified_unified_literals.insert(target_literal->accept(factoring_applicator));

                                unified_literals = std::move(simplified_unified_literals);
                                factoring_applicator.keep_working_set();
                                factored = true;
                                break;
                            }
                        }

                        if (trivial || factored)
                            break;
                    }
                } while (!trivial && factored);

                if (!trivial) {
                    /*
                     * A non-trivial clause produced through resolution should be considered a fresh resolvent. This is
                     * described by the source LHS and RHS clauses, the MGU, and the unified clause of the
                     * post-application literals as a clause under disjunction.
                     *
                     * TODO: make this nicer?  Does the resolvent need to store LHS and RHS clauses?
                     */
                    SentenceRoot::Clause unified_clause;
                    unified_clause.reserve(unified_literals.size());
                    unified_clause.insert(unified_clause.end(), unified_literals.begin(), unified_literals.end());

                    resolvents.emplace_back(lhs_clause, rhs_clause,
                        unification_visitor.observe_substitutions(), std::move(unified_clause));
                }

                applicator.keep_working_set();
            }

            // Any important MGUs have been copied into a resolvent.
            unification_visitor.reset_substitutions();
        }

    return resolvents;
}

} // namespace optifol
