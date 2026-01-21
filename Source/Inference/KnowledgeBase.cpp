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

void KnowledgeBase::ask(const SentenceRoot &query)
{
    tell(query); // TODO: Just for testing. Assume this is the negated query...

    for (const auto& clause : clauses)
        for (const auto& other_clause : clauses)
            priority_queue.push_back(resolve(clause, other_clause));
}

KnowledgeBase::Resolvent KnowledgeBase::resolve(const SentenceRoot::Clause &lhs_clause,
    const SentenceRoot::Clause &rhs_clause) const
{
    for (const auto lhs_literal : lhs_clause)
        for (const auto rhs_literal : rhs_clause) {

            // Attempt to unify the LHS literal with the negation of the RHS literal.

            auto rhs_args_copy = rhs_literal->observe_arguments();
            const Literal negated_rhs(std::string(rhs_literal->get_name()), std::move(rhs_args_copy),
                rhs_literal->is_negative_polarity());

            UnificationVisitor unification_visitor(symbol_repository);

            if (lhs_literal->accept(unification_visitor, negated_rhs)) {

                /*
                 * If the LHS and negated RHS can be unified, we have attained a most-general unifier. Construct the
                 * final resolvent according to the predicate resolution rule: apply the unifier to the union of all
                 * literals except the pair removed by unification. For the LHS and RHS clauses, over all literals
                 * except the pair removed by resolution, apply the MGU and add to the resolvent set.
                 *
                 * Note that application of the MGU might create new literals. In that case, they are added to the
                 * general symbol store so we only have to see non-owning, raw, immutable pointers.
                 */

                RawUnorderedSet<const Literal> unified_literals;
                const UnificationApplicationVisitor application_visitor(unification_visitor.observe_substitutions(),
                    symbol_repository);

                for (const auto lhs_other : lhs_clause)
                    if (lhs_other != lhs_literal)
                        unified_literals.insert(lhs_other->accept(application_visitor));

                for (const auto rhs_other : rhs_clause)
                    if (rhs_other != rhs_literal)
                        unified_literals.insert(rhs_other->accept(application_visitor));
            }
        }

    return {};
}

} // namespace optifol
