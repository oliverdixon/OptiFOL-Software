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

#include "../../../../Exceptions/SemanticException.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../../../IR/MutableVariants/Terms/MutableVariable.hpp"

namespace optifol
{

const char * SymbolStandardisingVisitor::visitor_name = "SymbolStandardiser";

std::string_view SymbolStandardisingVisitor::get_visitor_name() const
{
    return visitor_name;
}

void SymbolStandardisingVisitor::visit(MutableQuantified &node)
{
    /*
     * Open the scope, deal with the contents, and close it. Note that the act of opening a scope transfers ownership of
     * the bound term to the internal visitor state, hence it cannot be used until the scope is closed and the bound
     * term is returned. It is invariant that SymbolStandardisingVisitor::visit(MutableQuantified&) does attempt to
     * access its own bound term while its relevant scope is open.
     */
    const auto rule_reference = open_scope(node);
    MutatingSentenceVisitorBase::visit(node);
    close_scope(node, rule_reference);
}

void SymbolStandardisingVisitor::visit(MutablePredicate &node)
{
    auto &args = node.observe_arguments();
    const auto argument_count = args.size();

    // For each term argument, apply any relevant disambiguation rewriting. Then recurse with the term visitor.
    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto& rule = rewriting_rules.find(args[i]->get_disambiguated_name());
        if (rule != rewriting_rules.cend())
            args[i] = rule->second->clone();

        args[i]->accept(term_visitor);
    }
}

std::optional<decltype(SymbolStandardisingVisitor::rewriting_rules)::iterator> SymbolStandardisingVisitor::open_scope(
    MutableQuantified &node)
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
        node.put_bound_term(std::make_unique<MutableVariable>(original_name, new_name));
        scope.emplace(node.observe_bound_term()->to_string());
        return rewriting_rules.emplace(original_name, node.take_bound_term()).first;
    }

    scope.emplace(node.observe_bound_term()->to_string());
    return {};
}

void SymbolStandardisingVisitor::close_scope(MutableQuantified &node,
    const std::optional<decltype(SymbolStandardisingVisitor::rewriting_rules)::iterator> &rule_reference)
{
    if (rule_reference.has_value() && rule_reference != rewriting_rules.end()) {
        /*
         * If we were provided with a valid rule iterator reference, return the borrowed bound term to the quantifier.
         * If the optional container is empty, we assume that no rewriting rule was produced for this scope, and thus
         * there was no borrowed bound term that needs to be returned. This is verified with an assert to ensure that we
         * can get some observing reference to the bound term of the given quantified node.
         */
        node.put_bound_term(std::move(rule_reference->operator->()->second));
        rewriting_rules.erase(*rule_reference);
    }

    const auto bound_term = node.observe_bound_term();
    assert(bound_term != nullptr); // Ensure that the bound term has been correctly returned to the quantifier node.

    const auto& name = bound_term->to_string();
    assert(scope.contains(name)); // Ensure that the current scope is what we expect.

    scope.erase(name);
    adjacent.emplace(bound_term->get_disambiguated_name());
}

std::string SymbolStandardisingVisitor::generate_name(const std::string &name)
{
    return name + MutableVariable::disambiguating_delimiter + std::to_string(unique_name_counter++);
}

}
