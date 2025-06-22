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

#include "Terms/ITerm.hpp"

namespace optifol
{

const IProcessedTerm *SymbolRepository::add_symbol(std::unique_ptr<IProcessedTerm> &&term)
{
    const auto given_handle = term.get();
    auto [inserted_it, inserted] = terms.insert(std::move(term));

    if (!inserted) {
        const auto existing_it = terms.find(*given_handle);
        if (existing_it == terms.cend())
            throw std::runtime_error("Term " + std::string(given_handle->get_disambiguated_name()) +
                                     " could not be inserted or located in the symbol repository.");
        return existing_it->get();
    }

    return inserted_it->get();
}

const Predicate *SymbolRepository::add_symbol(std::unique_ptr<Predicate> &&predicate)
{
    const auto given_handle = predicate.get();
    auto [inserted_it, inserted] = predicates.insert(std::move(predicate));

    if (!inserted) {
        const auto existing_it = predicates.find(*given_handle);
        if (existing_it == predicates.cend())
            throw std::runtime_error("Sentence predicate " + std::string(given_handle->get_name()) +
                                     " could not be inserted or located in the symbol repository.");
        return existing_it->get();
    }

    return inserted_it->get();
}

std::optional<const IProcessedTerm *> SymbolRepository::get_symbol_handle(const IProcessedTerm &term) const
{
    const auto it = terms.find(term);
    if (it == terms.cend())
        return {};

    return it->get();
}

std::optional<const Predicate *> SymbolRepository::get_symbol_handle(const Predicate &predicate) const
{
    const auto it = predicates.find(predicate);
    if (it == predicates.cend())
        return {};

    return it->get();
}

} // namespace optifol
