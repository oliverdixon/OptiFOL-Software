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

const Variable *SymbolRepository::add_symbol(std::unique_ptr<Variable> &&variable)
{
    const auto find_it = variables.find(*variable);
    if (find_it != variables.cend())
        return find_it->get();

    const auto [inserted_it, success] = variables.insert(std::move(variable));
    if (!success)
        throw std::runtime_error("Cannot add variable: insertion failed.");

    return inserted_it->get();
}

const IProcessedTerm *SymbolRepository::get_symbol_handle(const IProcessedTerm &term) const
{
    const auto it = terms.find(term);
    if (it == terms.cend()) {
        const auto variable_try = dynamic_cast<const Variable *>(&term);
        return variable_try == nullptr ? nullptr : get_symbol_handle(*variable_try);
    }

    return it->get();
}

const ISentence *SymbolRepository::get_symbol_handle(const ISentence &sentence) const
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
