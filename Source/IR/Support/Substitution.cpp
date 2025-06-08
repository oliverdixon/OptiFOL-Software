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

}
