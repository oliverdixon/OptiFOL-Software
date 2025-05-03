/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Symbol-Standardising Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-12-01
 * @version Development
 */

#include <cassert>

#include "SymbolStandardisingVisitor.hpp"
#include "../../../Exceptions/SemanticException.hpp"
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"
#include "../../../IR/Sentences/PredicationNode.hpp"
#include "../../../IR/Sentences/IdentitySentenceNode.hpp"

namespace optifol
{

void SymbolStandardisingVisitor::visit(QuantifiedSentenceNode &node)
{
    // If there's any applicable rewriting rules, make the relevant substitution before continuing.
    const auto &rule = rewriting_rules.find(node.get_bound_variable()->get_disambiguated_name());
    if (rule != rewriting_rules.cend())
        node.swap_bound_variable(rule->second);

    // Open the scope, deal with the contents, and close it.
    open_scope(node);
    MutatingSentenceVisitorBase::visit(node);
    close_scope(*node.get_bound_variable());
}

void SymbolStandardisingVisitor::visit(PredicationNode &node)
{
    auto &args = node.arguments;
    const auto argument_count = args.size();

    // For each term argument, apply any relevant disambiguation rewriting. Then recurse with the term visitor.
    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto& rule = rewriting_rules.find(args[i]->get_disambiguated_name());
        if (rule != rewriting_rules.cend())
            args[i] = rule->second;

        args[i]->accept(term_visitor);
    }

    MutatingSentenceVisitorBase::visit(node);
}

void SymbolStandardisingVisitor::visit(IdentitySentenceNode &node)
{
    const auto& lhs_operand = node.get_lhs_operand();
    const auto& rhs_operand = node.get_rhs_operand();

    // Apply any relevant disambiguation rewriting to the LHS operand.
    const auto& lhs_rule = rewriting_rules.find(lhs_operand->get_disambiguated_name());
    if (lhs_rule != rewriting_rules.cend())
        node.swap_lhs_operand(lhs_rule->second);

    // Apply any relevant disambiguation rewriting to the RHS operand.
    const auto& rhs_rule = rewriting_rules.find(rhs_operand->get_disambiguated_name());
    if (rhs_rule != rewriting_rules.cend())
        node.swap_rhs_operand(rhs_rule->second);

    // Recurse down both branches, with the disambiguated names.
    node.get_lhs_operand()->accept(term_visitor);
    node.get_rhs_operand()->accept(term_visitor);

    MutatingSentenceVisitorBase::visit(node);
}

void SymbolStandardisingVisitor::reset()
{
    scope.clear();
    adjacent.clear();
    rewriting_rules.clear();
    unique_name_counter = 0;
}

void SymbolStandardisingVisitor::open_scope(QuantifiedSentenceNode &node)
{
    const auto& original_name = node.get_bound_variable()->to_string();

    if (scope.contains(original_name))
        throw SemanticException("Declared variable \"" + original_name + "\" is already defined in the current scope.");

    if (adjacent.contains(original_name)) {
        auto new_name = generate_name(original_name);

        while (adjacent.contains(new_name))
            new_name = generate_name(new_name);

        /* In addition to updating the scope set, we also manage the rewriting rules table, since an entry would only
         * appear given a prospectively ambiguous variable node, which clashes with a bound variable in an adjacent
         * scope. */
        auto new_variable = std::make_shared<VariableNode>(original_name, new_name);
        node.swap_bound_variable(new_variable);
        rewriting_rules.emplace(original_name, std::move(new_variable));
    }

    scope.emplace(node.get_bound_variable()->to_string());
}

void SymbolStandardisingVisitor::close_scope(const VariableNode &node)
{
    const auto& name = node.to_string();
    assert(scope.contains(name));

    scope.erase(name);
    rewriting_rules.erase(name);
    adjacent.emplace(node.get_disambiguated_name());
}

std::string SymbolStandardisingVisitor::generate_name(const std::string &name)
{
    return name + std::to_string(unique_name_counter++);
}

}
