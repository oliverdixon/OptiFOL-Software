/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "VariableNode.hpp"

#include "../../Visitors/Terms/MutatingTermVisitorBase.hpp"
#include "FunctionNode.hpp"

namespace optifol
{

VariableNode::VariableNode(std::string name):
    name(std::move(name))
{}

VariableNode::VariableNode(std::string name, const std::string &disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(disambiguated_name)
{}

std::unique_ptr<ITermNode> VariableNode::clone() const
{
    if (disambiguated_name.has_value())
        return std::make_unique<VariableNode>(name, *disambiguated_name);

    return std::make_unique<VariableNode>(name);
}

std::string VariableNode::to_string() const
{
    return name;
}

std::string VariableNode::get_disambiguated_name() const
{
    return disambiguated_name.value_or(name);
}

void VariableNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

bool VariableNode::unify_with_me(const VariableNode &variable)
{
    if (hash() == variable.hash())
        // If atomics (e.g. variables) are trivially identical, they can be unified without an explicit substitution.
        return true;

    if (unify_substitution.has_value()) {
        // If the given variable already has a binding, ensure that its bound mapping can be unified with ourselves.
        const auto &my_binding_it = unify_substitution->bindings.find(*this);
        if (my_binding_it != unify_substitution->bindings.cend())
            return static_cast<UnifyCandidateBase &>(my_binding_it->second).unify_with_me(variable);
    }

    const auto &their_binding_it = unify_substitution->bindings.find(variable);
    if (their_binding_it != unify_substitution->bindings.cend())
        return static_cast<UnifyCandidateBase *>(this)->unify_with_me(their_binding_it->second);

    // TODO: occurs check

    // If all checks pass, we can do a unification. Register the substitution and indicate success.
    register_substitution(variable, *this);
    return true;
}

bool VariableNode::unify_with_me(ITermNode &term)
{
    if (hash() == term.hash())
        // If atomics (e.g. variables) are trivially identical, they can be unified without an explicit substitution.
            return true;

    if (unify_substitution.has_value()) {
        // If the given variable already has a binding, ensure that its bound mapping can be unified with ourselves.
        const auto &my_binding_it = unify_substitution->bindings.find(*this);
        if (my_binding_it != unify_substitution->bindings.cend())
            return static_cast<UnifyCandidateBase &>(my_binding_it->second).unify_with_me(term);
    }

    // TODO: occurs check

    // If all checks pass, we can do a unification. Register the substitution and indicate success.
    register_substitution(*this, term);
    return true;
}

} // namespace optifol
