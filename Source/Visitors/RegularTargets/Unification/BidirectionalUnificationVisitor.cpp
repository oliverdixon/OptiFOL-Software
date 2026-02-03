/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the FOL binary-unification visitor (bidirectional)
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#include "BidirectionalUnificationVisitor.hpp"

#include "../../../IR/SymbolRepository.hpp"
#include "../../../IR/Terms/Constant.hpp"
#include "../../../IR/Terms/ITerm.hpp"
#include "../../../IR/Terms/Variable.hpp"
#include "UnificationApplicationVisitor.hpp"

namespace optifol
{

BidirectionalUnificationVisitor::BidirectionalUnificationVisitor(std::shared_ptr<SymbolRepository> symbol_repository) :
    UnificationVisitor(std::move(symbol_repository)),
    application_visitor(substitutions, this->symbol_repository)
{
}

bool BidirectionalUnificationVisitor::visit(const Constant &constant_lhs, const Variable &variable_rhs)
{
    return UnificationVisitor::visit(variable_rhs, constant_lhs);
}

bool BidirectionalUnificationVisitor::visit(const Function &function_lhs, const Variable &variable_rhs)
{
    return UnificationVisitor::visit(variable_rhs, function_lhs);
}

bool BidirectionalUnificationVisitor::variable_generic(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs)
{
    if (variable_lhs.hash() == generic_term_rhs.hash())
        // If atomics (e.g. variables) are trivially identical, they can be unified without an explicit substitution.
        return true;

    /*
     * If the given LHS variable already has a binding, ensure that its bound mapping can be unified with the other
     * variable.
     */
    const auto &lhs_binding_it = substitutions->unifier.find(variable_lhs);
    if (lhs_binding_it != substitutions->unifier.cend())
        return lhs_binding_it->second->accept(*this, generic_term_rhs);

    // Perform an 'occurs check' only when considering binding a variable to a generic, non-variable term.
    if (occurs_check(variable_lhs, generic_term_rhs))
        return false;

    // If all checks pass, we can do a unification between the variables. Register the replacement and indicate success.
    register_substitution(variable_lhs, generic_term_rhs);
    return true;
}

bool BidirectionalUnificationVisitor::occurs_check(const Variable &variable_lhs, const IProcessedTerm &generic_term_rhs)
{
    const auto applied_term = generic_term_rhs.accept(application_visitor);

    const bool is_self_nested = *applied_term == generic_term_rhs ?
        generic_term_rhs.is_self_nested(variable_lhs) :
        applied_term->is_self_nested(variable_lhs);

    application_visitor.discard_new_symbols();
    return is_self_nested;
}

} // namespace optifol
