/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the SymbolRepository class
 * @author Oliver Dixon
 * @date 2025-06-13
 * @version Development
 */

#include "SymbolRepository.hpp"

namespace optifol
{

const IProcessedTerm *SymbolRepository::get_symbol_handle(const IProcessedTerm &term) const
{
    const auto it = terms.find(term);
    if (it == terms.cend())
        return {};

    return it->get();
}

const ISentence *SymbolRepository::get_symbol_handle(const ISentence &sentence) const
{
    const auto it = sentences.find(sentence);
    if (it == sentences.cend())
        return {};

    return it->get();
}

bool SymbolRepository::operator==(const SymbolRepository & other) const noexcept
{
    return sentences == other.sentences && terms == other.terms;
}

} // namespace optifol
