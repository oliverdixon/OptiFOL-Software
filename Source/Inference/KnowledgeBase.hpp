/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 9/9/25.
//

#ifndef OPTIFOL_KNOWLEDGEBASE_HPP
#define OPTIFOL_KNOWLEDGEBASE_HPP

#include "../IR/Sentences/SentenceRoot.hpp"

namespace optifol
{

class SymbolRepository;

class KnowledgeBase
{
public:
    explicit KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository);

    void tell(const SentenceRoot::Clause& clause);

private:
    std::vector<SentenceRoot::Clause> clauses;

    std::shared_ptr<SymbolRepository> symbol_repository;
};

} // namespace optifol

#endif // OPTIFOL_KNOWLEDGEBASE_HPP
