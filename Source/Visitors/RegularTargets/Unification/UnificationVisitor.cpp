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

#include "../../../IR/Sentences/Predicate.hpp"
#include "../../../IR/Terms/Constant.hpp"
#include "../../../IR/Terms/Function.hpp"
#include "../../../IR/Terms/ITerm.hpp"
#include "../../../IR/Terms/Variable.hpp"

namespace optifol
{

bool UnificationVisitor::visit(const Predicate &predicate_lhs, const Predicate &predicate_rhs)
{
    const auto &lhs_arguments = predicate_lhs.observe_arguments();
    const auto &rhs_arguments = predicate_rhs.observe_arguments();
    const auto argument_count = lhs_arguments.size();

    if (predicate_lhs.get_name() != predicate_rhs.get_name() || argument_count != rhs_arguments.size())
        // Cannot unify if predicates are fundamentally different, i.e. different name or number of arguments.
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx)
        if (lhs_arguments[argument_idx]->accept(*this, *rhs_arguments[argument_idx]) == false)
            // If zipped/pairwise arguments cannot be independently term-unified, the functions cannot be unified.
            return false;

    return true;
}

bool UnificationVisitor::visit(const Variable &variable_lhs, const Constant &constant_rhs)
{
    return variable_generic(variable_lhs, constant_rhs);
}

bool UnificationVisitor::visit(const Variable &variable_lhs, const Function &function_rhs)
{
    return variable_generic(variable_lhs, function_rhs);
}

bool UnificationVisitor::visit(const Variable &variable_lhs, const Variable &variable_rhs)
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

bool UnificationVisitor::visit(const Function &function_lhs, const Function &function_rhs)
{
    const auto &lhs_arguments = function_lhs.observe_arguments();
    const auto &rhs_arguments = function_rhs.observe_arguments();
    const auto argument_count = lhs_arguments.size();

    if (function_lhs.get_disambiguated_name() != function_rhs.get_disambiguated_name() ||
            argument_count != function_rhs.observe_arguments().size())
        // Cannot unify if functions are fundamentally different.
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx)
        if (lhs_arguments[argument_idx]->accept(*this, *rhs_arguments[argument_idx]) == false)
            // If zipped/pairwise arguments cannot be independently term-unified, the functions cannot be unified.
            return false;

    return true;
}

const std::optional<Substitution> &UnificationVisitor::observe_substitutions() const
{
    return substitutions;
}

bool UnificationVisitor::variable_generic(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs)
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

void UnificationVisitor::register_substitution(const Variable &bound_key, const IProcessedTerm &bound_value)
{
    const auto wrapped_variable = std::cref(bound_key);
    // substitution_keys.push_back(wrapped_variable); // TODO value in pair will be dangling if term node is deleted.
    if (substitutions.has_value() == false)
        substitutions.emplace();
    substitutions->bindings.emplace(wrapped_variable, std::ref(bound_value));
}

bool UnificationVisitor::occurs_check(const Variable &variable_lhs, const Variable &variable_rhs)
{
    return variable_lhs == variable_rhs;
}

bool UnificationVisitor::occurs_check(const Variable &variable_lhs, const Function &function_rhs)
{
    const auto& arguments = function_rhs.observe_arguments();
    for (const auto& arg : function_rhs.observe_arguments())
        if ()
}

} // namespace optifol
