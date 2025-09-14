/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 9/9/25.
//

#include "KnowledgeBase.hpp"

namespace optifol
{

KnowledgeBase::KnowledgeBase(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository))
{
}

void KnowledgeBase::tell(const SentenceRoot::Clause &clause)
{
    clauses.push_back(clause);
}

} // namespace optifol
