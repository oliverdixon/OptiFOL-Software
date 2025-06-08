/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Variable-Term Substitution Map
 * @author Oliver Dixon
 * @date 2025-06-07
 * @version Development
 */

#include "Substitution.hpp"

#include "../Terms/VariableNode.hpp"

namespace optifol
{

std::size_t Substitution::KeyRefHashingFunctor::operator()(const std::reference_wrapper<const VariableNode> object)
        const noexcept
{
    return object.get().hash();
}

bool Substitution::KeyRefEqualityFunctor::operator()(
        const std::reference_wrapper<const VariableNode> lhs_object,
        const std::reference_wrapper<const VariableNode> rhs_object) const noexcept
{
    return lhs_object.get().hash() == rhs_object.get().hash();
}

Substitution::Substitution(std::initializer_list<std::pair<const VariableNode &, const ITermNode &>> entries)
{
    for (const auto& [key, value] : entries)
        // TODO value in pair will be dangling if term node is deleted.
        bindings.emplace(std::cref(key), std::cref(value));
}

bool Substitution::operator==(const Substitution &substitution) const
{
    const auto n_entries = bindings.size();
    if (n_entries != substitution.bindings.size())
        return false;

    for (const auto& [variable, binding] :
            bindings) {
        const auto dest_it = substitution.bindings.find(variable);
        if (dest_it == substitution.bindings.cend() || dest_it->second.get().hash() != binding.get().hash())
            return false;
    }

    return true;
}

}
