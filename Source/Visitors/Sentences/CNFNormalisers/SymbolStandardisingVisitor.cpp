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

#include <algorithm>

#include "../../../Exceptions/SemanticException.hpp"
#include "../../../IR/Sentences/IdentitySentenceNode.hpp"
#include "../../../IR/Sentences/PredicationNode.hpp"
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"

namespace optifol
{

void SymbolStandardisingVisitor::visit(QuantifiedSentenceNode &node)
{
    // Open the scope, deal with the contents, and close it.
    open_scope(node);
    MutatingSentenceVisitorBase::visit(node);
    close_scope(node.observe_bound_term());
}

void SymbolStandardisingVisitor::visit(PredicationNode &node)
{
    auto &args = node.arguments;
    const auto argument_count = args.size();

    // For each term argument, apply any relevant disambiguation rewriting. Then recurse with the term visitor.
    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto& rule = rewriting_rules.find(args[i]->get_disambiguated_name());
        if (rule != rewriting_rules.cend())
            /*
             * TODO: should we be cloning here? Shouldn't the same variable be stored once and referred to with
             *  shared_ptr?
             */
            args[i] = rule->second->clone();

        args[i]->accept(term_visitor);
    }
}

void SymbolStandardisingVisitor::visit(IdentitySentenceNode &node)
{
    auto borrowed_lhs = node.take_lhs_operand();

    // Apply any relevant disambiguation rewriting to the LHS operand.
    const auto& lhs_rule = rewriting_rules.find(borrowed_lhs->get_disambiguated_name());
    if (lhs_rule != rewriting_rules.cend())
        node.swap_lhs_operand(lhs_rule->second->clone());

    borrowed_lhs->accept(term_visitor);
    node.put_lhs_operand(std::move(borrowed_lhs));

    auto borrowed_rhs = node.take_rhs_operand();

    // Apply any relevant disambiguation rewriting to the RHS operand.
    const auto& rhs_rule = rewriting_rules.find(borrowed_rhs->get_disambiguated_name());
    if (rhs_rule != rewriting_rules.cend())
        node.swap_rhs_operand(rhs_rule->second->clone());

    borrowed_rhs->accept(term_visitor);
    node.put_rhs_operand(std::move(borrowed_rhs));
}

void SymbolStandardisingVisitor::open_scope(QuantifiedSentenceNode &node)
{
    const auto& original_name = node.observe_bound_term()->to_string();

    if (scope.contains(original_name))
        throw SemanticException("Declared variable \"" + original_name + "\" is already defined in the current scope.");

    if (adjacent.contains(original_name)) {
        // If an adjacent scope has already used a variable with this name, it is ambiguous and needs renaming.
        auto new_name = generate_name(original_name);

        while (adjacent.contains(new_name))
            // Repeatedly disambiguate until we have something unique.
            new_name = generate_name(new_name);

        /* In addition to updating the scope set, we also manage the rewriting rules table, since an entry would only
         * appear given a prospectively ambiguous variable node, which clashes with a bound variable in an adjacent
         * scope. */
        node.put_bound_term(std::make_unique<VariableNode>(original_name, new_name));
        rewriting_rules.emplace(original_name, node.observe_bound_term());
    }

    scope.emplace(node.observe_bound_term()->to_string());
}

void SymbolStandardisingVisitor::close_scope(const ITermNode * node)
{
    const auto& name = node->to_string();
    assert(scope.contains(name));

    scope.erase(name);
    rewriting_rules.erase(name);
    adjacent.emplace(node->get_disambiguated_name());
}

std::string SymbolStandardisingVisitor::generate_name(const std::string &name)
{
    return name + std::to_string(unique_name_counter++);
}

}
