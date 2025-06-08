/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition for the generic IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "ITermNode.hpp"
#include "VariableNode.hpp"

namespace optifol
{

ITermNode::~ITermNode()
{
    for (const auto key: substitution_keys)
        unify_substitution->bindings.erase(key);
}

bool ITermNode::unify_with_me(const VariableNode &variable)
{
    if (hash() == variable.hash())
        // If atomics (e.g. variables) are trivially identical, they can be unified without an explicit substitution.
        return true;

    if (unify_substitution.has_value()) {
        // If the given variable already has a binding, ensure that its bound mapping can be unified with ourselves.
        const auto &binding_it = unify_substitution->bindings.find(variable);
        if (binding_it != unify_substitution->bindings.cend())
            return static_cast<UnifyCandidateBase &>(binding_it->second).unify_with_me(*this);
    }

    // TODO: occurs check on us vs. unifier applied on variable

    // If all checks pass, we can do a unification. Register the substitution and indicate success.
    register_substitution(variable, *this);
    return true;
}

void ITermNode::register_substitution(const VariableNode &bound_key, ITermNode &bound_value)
{
    const auto wrapped_variable = std::cref(bound_key);
    substitution_keys.push_back(wrapped_variable);
    if (unify_substitution.has_value() == false)
        unify_substitution.emplace();
    unify_substitution->bindings.emplace(wrapped_variable, std::ref(bound_value));
}

} // namespace optifol
