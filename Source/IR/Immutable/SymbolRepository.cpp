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

const IMutableTermNode *SymbolRepository::add_symbol(std::unique_ptr<IMutableTermNode> &&term)
{
    const auto given_handle = term.get();
    auto [inserted_it, inserted] = terms.insert(std::move(term));

    if (!inserted) {
        const auto existing_it = terms.find(*given_handle);
        if (existing_it == terms.cend())
            throw std::runtime_error("Term " + given_handle->get_disambiguated_name() + " could not be inserted or "
                                        "located in the symbol repository.");
        return existing_it->get();
    }

    return inserted_it->get();
}

const MutablePredicationNode *SymbolRepository::add_symbol(std::unique_ptr<MutablePredicationNode> &&predicate)
{
    const auto given_handle = predicate.get();
    auto [inserted_it, inserted] = predicates.insert(std::move(predicate));

    if (!inserted) {
        const auto existing_it = predicates.find(*given_handle);
        if (existing_it == predicates.cend())
            throw std::runtime_error("Sentence predicate " + given_handle->name + " could not be inserted or located "
                                        "in the symbol repository.");
        return existing_it->get();
    }

    return inserted_it->get();
}

std::optional<const IMutableTermNode *> SymbolRepository::get_symbol_handle(const IMutableTermNode &term) const
{
    const auto it = terms.find(term);
    if (it == terms.cend())
        return {};

    return it->get();
}

std::optional<const MutablePredicationNode *> SymbolRepository::get_symbol_handle(const MutablePredicationNode &predicate) const
{
    const auto it = predicates.find(predicate);
    if (it == predicates.cend())
        return {};

    return it->get();
}

}
