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
    if (it == terms.cend()) {
        const auto variable_try = dynamic_cast<const Variable *>(&term);
        return variable_try == nullptr ? nullptr : get_symbol_handle(*variable_try);
    }

    return it->get();
}

const IProcessedSentence *SymbolRepository::get_symbol_handle(const IProcessedSentence &sentence) const
{
    const auto it = sentences.find(sentence);
    if (it == sentences.cend())
        return nullptr;

    return it->get();
}

const Variable *SymbolRepository::get_symbol_handle(const Variable &variable) const
{
    const auto it = variables.find(variable);
    if (it == variables.cend())
        return nullptr;

    return it->get();
}

bool SymbolRepository::operator==(const SymbolRepository & other) const noexcept
{
    return sentences == other.sentences && terms == other.terms;
}

} // namespace optifol
