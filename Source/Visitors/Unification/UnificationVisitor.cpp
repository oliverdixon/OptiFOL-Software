/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the FOL binary-unification visitor
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#include "UnificationVisitor.hpp"
#include "../../IR/Mutable/Sentences/MutablePredicationNode.hpp"
#include "../../IR/Mutable/Terms/MutableFunctionNode.hpp"
#include "../../IR/Mutable/Terms/IMutableTermNode.hpp"
#include "../../IR/Mutable/Terms/MutableVariableNode.hpp"

namespace optifol
{

bool UnificationVisitor::visit(const MutablePredicationNode &predicate_lhs, const MutablePredicationNode &predicate_rhs)
{
    const auto argument_count = predicate_lhs.arguments.size();

    if (predicate_lhs.name != predicate_rhs.name || argument_count != predicate_rhs.arguments.size())
        // Cannot unify if predicates are fundamentally different.
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx)
        if (predicate_lhs.arguments[argument_idx]->accept(*this, *predicate_rhs.arguments[argument_idx]) == false)
            // If zipped/pairwise arguments cannot be independently term-unified, the functions cannot be unified.
            return false;

    return true;
}

bool UnificationVisitor::visit(const MutableVariableNode &variable_lhs, const IMutableTermNode &generic_term_rhs)
{
    if (variable_lhs.hash() == generic_term_rhs.hash())
        // If atomics (e.g. variables) are trivially identical, they can be unified without an explicit substitution.
        return true;

    if (substitutions.has_value()) {
        /*
         * If the given LHS variable already has a binding, ensure that its bound mapping can be unified with the other
         * variable.
         */
        const auto &lhs_binding_it = substitutions->bindings.find(variable_lhs);
        if (lhs_binding_it != substitutions->bindings.cend())
            return lhs_binding_it->second.get().accept(*this, generic_term_rhs);
    }

    // TODO: occurs check

    // If all checks pass, we can do a unification between the variables. Register the replacement and indicate success.
    register_substitution(variable_lhs, generic_term_rhs);
    return true;
}

bool UnificationVisitor::visit(const MutableVariableNode &variable_lhs, const MutableVariableNode &variable_rhs)
{
    if (variable_lhs.hash() == variable_rhs.hash())
        // If atomics (e.g. variables) are trivially identical, they can be unified without an explicit substitution.
        return true;

    if (substitutions.has_value()) {
        /*
         * If the given LHS variable already has a binding, ensure that its bound mapping can be unified with the other
         * variable.
         */
        const auto &lhs_binding_it = substitutions->bindings.find(variable_lhs);
        if (lhs_binding_it != substitutions->bindings.cend())
            return lhs_binding_it->second.get().accept(*this, variable_rhs);

        /*
         * Repeat the above check for RHS; due to static binding, we know it's a variable and not a generic term. Thus
         * it could be a key in the substitutions map.
         */
        const auto &rhs_binding_it = substitutions->bindings.find(variable_rhs);
        if (rhs_binding_it != substitutions->bindings.cend())
            return rhs_binding_it->second.get().accept(*this, variable_lhs);
    }

    // TODO: occurs check

    // If all checks pass, we can do a unification between the variables. Register the replacement and indicate success.
    register_substitution(variable_lhs, variable_rhs);
    return true;
}

bool UnificationVisitor::visit(const MutableFunctionNode &function_lhs, const MutableFunctionNode &function_rhs)
{
    const auto argument_count = function_lhs.observe_arguments().size();

    if (function_lhs.get_disambiguated_name() != function_rhs.get_disambiguated_name() ||
        argument_count != function_rhs.observe_arguments().size())
        // Cannot unify if functions are fundamentally different.
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx)
        if (function_lhs.observe_arguments()[argument_idx]->accept(
                    *this, *function_rhs.observe_arguments()[argument_idx]) == false)
            // If zipped/pairwise arguments cannot be independently term-unified, the functions cannot be unified.
            return false;

    return true;
}

bool UnificationVisitor::visit(const IMutableTermNode &generic_term_lhs, const MutableFunctionNode &function_rhs)
{
    std::ignore = generic_term_lhs;
    std::ignore = function_rhs;

    return false;
}

bool UnificationVisitor::visit(const IMutableTermNode &generic_term_lhs, const IMutableTermNode &generic_term_rhs)
{
    std::ignore = generic_term_lhs;
    std::ignore = generic_term_rhs;

    return false;
}

const std::optional<Substitution> &UnificationVisitor::observe_substitutions() const
{
    return substitutions;
}

void UnificationVisitor::register_substitution(const MutableVariableNode &bound_key, const IMutableTermNode &bound_value)
{
    const auto wrapped_variable = std::cref(bound_key);
    // substitution_keys.push_back(wrapped_variable); // TODO value in pair will be dangling if term node is deleted.
    if (substitutions.has_value() == false)
        substitutions.emplace();
    substitutions->bindings.emplace(wrapped_variable, std::ref(bound_value));
}

}
