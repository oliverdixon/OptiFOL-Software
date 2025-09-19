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

#include <deque>


#include "../IR/Sentences/SentenceRoot.hpp"

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

    void ask(const SentenceRoot& query);

private:
    using Resolvent = std::pair<const Variable *, const IProcessedTerm *>;

    Resolvent resolve(const SentenceRoot::Clause& lhs_clause, const SentenceRoot::Clause& rhs_clause) const;

    std::vector<SentenceRoot::Clause> clauses;
    std::deque<Resolvent> priority_queue;

    std::shared_ptr<SymbolRepository> symbol_repository;
};

} // namespace optifol

#endif // OPTIFOL_KNOWLEDGEBASE_HPP
