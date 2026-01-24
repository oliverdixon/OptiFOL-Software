/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL Resolution Knowledge Base
 * @author Oliver Dixon
 * @date 2025-09-14
 * @version Development
 */

#ifndef OPTIFOL_KNOWLEDGEBASE_HPP
#define OPTIFOL_KNOWLEDGEBASE_HPP

#include "../IR/Terms/Variable.hpp"
#include "../IR/Sentences/SentenceRoot.hpp"
#include "../Visitors/RegularTargets/Unification/Unifier.hpp"

namespace optifol
{

class SymbolRepository;
class Variable;
class IProcessedTerm;

class KnowledgeBase
{
public:
    explicit KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository);

    void tell(const SentenceRoot& sentence);

    void tell(const SentenceRoot::Clause& clause);

    bool query_negative(const SentenceRoot &negated_query);

private:
    struct Resolvent
    {
        SentenceRoot::Clause lhs_clause;
        SentenceRoot::Clause rhs_clause;
        Unifier resolving_unifier;
        SentenceRoot::Clause unified_clause;

        friend bool operator<(const Resolvent& lhs, const Resolvent& rhs) noexcept
        {
            const auto lhs_has_unit = lhs.lhs_clause.size() == 1 || lhs.rhs_clause.size() == 1;
            const auto rhs_has_unit = rhs.lhs_clause.size() == 1 || rhs.rhs_clause.size() == 1;

            if (lhs_has_unit && !rhs_has_unit)
                return false; // LHS has higher priority.

            if (!lhs_has_unit && rhs_has_unit)
                return true; // RHS has higher priority.

            return false; // Equal priority.
        }
    };

    [[nodiscard]] std::vector<Resolvent> find_resolvents(
            const SentenceRoot::Clause &lhs_clause, const SentenceRoot::Clause &rhs_clause) const;

    std::vector<SentenceRoot::Clause> clauses;
    std::shared_ptr<SymbolRepository> symbol_repository;
};

} // namespace optifol

#endif // OPTIFOL_KNOWLEDGEBASE_HPP
