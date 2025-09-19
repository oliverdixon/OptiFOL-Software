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

#include "../../../Exceptions/SemanticException.hpp"
#include "../../../IR/Sentences/Literal.hpp"
#include "../../../IR/SymbolRepository.hpp"
#include "../../../IR/Terms/Constant.hpp"
#include "../../../IR/Terms/Function.hpp"
#include "../../../IR/Terms/ITerm.hpp"
#include "../../../IR/Terms/Variable.hpp"
#include "UnificationApplicationVisitor.hpp"

namespace optifol
{

UnificationVisitor::UnificationVisitor(std::shared_ptr<SymbolRepository> symbol_repository) :
    symbol_repository(std::move(symbol_repository))
{
}

bool UnificationVisitor::visit(const Literal &predicate_lhs, const Literal &predicate_rhs)
{
    const auto &lhs_arguments = predicate_lhs.observe_arguments();
    const auto &rhs_arguments = predicate_rhs.observe_arguments();
    const auto argument_count = lhs_arguments.size();

    if (predicate_lhs.is_negative_polarity() != predicate_rhs.is_negative_polarity())
        return false;

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

    /*
     * If the given LHS variable already has a binding, ensure that its bound mapping can be unified with the candidate
     * RHS variable.
     */
    const auto &lhs_binding_it = substitutions.find(variable_lhs);
    if (lhs_binding_it != substitutions.cend())
        return lhs_binding_it->second->accept(*this, variable_rhs);

    /*
     * Repeat the above check for RHS; due to static binding, we know it's a variable and not a generic term. Thus
     * it could be a key in the substitutions map. Ensure that the RHS binding, if it exists, can be bound with the
     * candidate LHS variable.
     */
    const auto &rhs_binding_it = substitutions.find(variable_rhs);
    if (rhs_binding_it != substitutions.cend())
        return rhs_binding_it->second->accept(*this, variable_lhs);

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

const UnificationVisitor::SubstitutionMap &UnificationVisitor::get_substitutions() const noexcept
{
    return substitutions;
}

bool UnificationVisitor::variable_generic(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs)
{
    if (variable_lhs.hash() == generic_term_rhs.hash())
        // If atomics (e.g. variables) are trivially identical, they can be unified without an explicit substitution.
        return true;

    /*
     * If the given LHS variable already has a binding, ensure that its bound mapping can be unified with the other
     * variable.
     */
    const auto &lhs_binding_it = substitutions.find(variable_lhs);
    if (lhs_binding_it != substitutions.cend())
        return lhs_binding_it->second->accept(*this, generic_term_rhs);

    // Perform an 'occurs check' only when considering binding a variable to a generic, non-variable term.
    if (occurs_check(variable_lhs, generic_term_rhs))
        return false;

    // If all checks pass, we can do a unification between the variables. Register the replacement and indicate success.
    register_substitution(variable_lhs, generic_term_rhs);
    return true;
}

void UnificationVisitor::register_substitution(const Variable &bound_key, const IProcessedTerm &bound_value)
{
    const auto variable_repo_ptr = symbol_repository->get_symbol_handle(bound_key);
    const auto bound_repo_ptr = symbol_repository->get_symbol_handle(bound_value);

    if (variable_repo_ptr == nullptr || bound_repo_ptr == nullptr)
        throw SemanticException("Attempted to register substitution for " + bound_key.to_string() + " but the "
            "Repository is incomplete.");

    substitutions.emplace(variable_repo_ptr, bound_repo_ptr);
}

bool UnificationVisitor::occurs_check(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs) const
{
    const UnificationApplicationVisitor applicator(substitutions, occurs_dummy_symbol_repo);
    auto application_variant = generic_term_rhs.accept(applicator);

    if (std::holds_alternative<std::unique_ptr<IProcessedTerm>>(application_variant)) {
        /*
         * Presence of a unique_ptr indicates that a new term was created solely to express the result of substitution.
         * Such terms are not added to the central symbol repository by the application visitor, so we have ownership
         * and destruct at the end of the scope once determining occurrence.
         */
        return std::get<std::unique_ptr<IProcessedTerm>>(application_variant)->is_self_nested(variable_lhs);
    }

    if (std::holds_alternative<const IProcessedTerm *>(application_variant)) {
        /*
         * Presence of a raw observing pointer indicates that an existing term was pulled from the central symbol
         * repository, as no substitutions were applicable, or they were sufficiently trivial to express with the given
         * immutable node. We don't have ownership of anything.
         */
        const auto temp_sub = std::get<const IProcessedTerm *>(application_variant);
        return temp_sub == nullptr ? generic_term_rhs.is_self_nested(variable_lhs) :
            temp_sub->is_self_nested(variable_lhs);
    }

    return false;
}

} // namespace optifol
