/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Existential Shifting Visitor
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#include "SkolemIntroducingVisitor.hpp"

#include <cassert>

#include "../../../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableIdentity.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../../../../IR/MutableVariants/Terms/MutableSkolemFunction.hpp"

namespace optifol
{

const char * SkolemIntroducingVisitor::visitor_name = "SkolemIntroduction";

SkolemIntroducingVisitor::SkolemIntroducingVisitor()
{
    universally_quantified_variables.emplace();
}

SkolemIntroducingVisitor::~SkolemIntroducingVisitor()
{
    assert(skolem_replacements.empty());
    assert(universally_quantified_variables.size() == 1 && universally_quantified_variables.top().empty());
}

std::string_view SkolemIntroducingVisitor::get_visitor_name() const
{
    return visitor_name;
}

void SkolemIntroducingVisitor::visit(MutableQuantified &node)
{
    auto borrowed_sentence = node.take_sentence();

    switch (node.get_quantifier_type()) {
    case QuantifierTypes::Universal:
        open_scope(node);
        borrowed_sentence->accept(*this);
        close_latest_scope(node);

        if (extracted_sentence == nullptr)
            node.put_sentence(std::move(borrowed_sentence));
        else
            node.put_sentence(std::move(extracted_sentence));

        break;

    case QuantifierTypes::Existential:
        eliminate_existential(*node.observe_bound_term());
        borrowed_sentence->accept(*this);
        extracted_sentence = std::move(borrowed_sentence);

        break;
    }
}

void SkolemIntroducingVisitor::visit(MutablePredicate &node)
{
    if (skolem_replacements.empty())
        return;

    auto &args = node.observe_arguments();
    const auto argument_count = args.size();

    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto& potential_replacement =
            skolem_replacements.find(args[i]->get_disambiguated_name());
        if (potential_replacement != skolem_replacements.cend())
            args[i] = potential_replacement->second->clone();

        args[i]->accept(term_visitor);
    }
}

void SkolemIntroducingVisitor::visit(MutableIdentity &node)
{
    auto borrowed_lhs = node.take_lhs_operand();

    // Apply any relevant disambiguation rewriting to the LHS operand.
    const auto& lhs_rule =
        skolem_replacements.find(borrowed_lhs->get_disambiguated_name());
    if (lhs_rule != skolem_replacements.cend())
        node.put_lhs_operand(lhs_rule->second->clone());

    borrowed_lhs->accept(term_visitor);
    node.put_lhs_operand(std::move(borrowed_lhs));

    auto borrowed_rhs = node.take_rhs_operand();

    // Apply any relevant disambiguation rewriting to the RHS operand.
    const auto& rhs_rule =
        skolem_replacements.find(borrowed_rhs->get_disambiguated_name());
    if (rhs_rule != skolem_replacements.cend())
        node.put_rhs_operand(rhs_rule->second->clone());

    borrowed_rhs->accept(term_visitor);
    node.put_rhs_operand(std::move(borrowed_rhs));
}

void SkolemIntroducingVisitor::visit(MutableBinaryConnected &node)
{
    auto borrowed_operand = node.take_lhs_operand();
    borrowed_operand->accept(*this);
    if (extracted_sentence == nullptr)
        node.put_lhs_operand(std::move(borrowed_operand));
    else
        node.put_lhs_operand(std::move(extracted_sentence));

    borrowed_operand = node.take_rhs_operand();
    borrowed_operand->accept(*this);
    if (extracted_sentence == nullptr)
        node.put_rhs_operand(std::move(borrowed_operand));
    else
        node.put_rhs_operand(std::move(extracted_sentence));
}

void SkolemIntroducingVisitor::visit(MutableSentenceRoot &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    if (extracted_sentence == nullptr)
        node.put_sentence(std::move(borrowed_sentence));
    else
        node.put_sentence(std::move(extracted_sentence));

    skolem_replacements.clear();
}

void SkolemIntroducingVisitor::open_scope(MutableQuantified &node)
{
    universally_quantified_variables.top().push_back(node.take_bound_term());
}

void SkolemIntroducingVisitor::close_latest_scope(MutableQuantified &node)
{
    const auto scoped_var_count = universally_quantified_variables.top().size();
    assert(scoped_var_count > 0); // We assume to be within a scope, and thus must have at least one bound variable.

    // Return the latest variable from the latest scope to the given node, assumed to be its original owner.
    node.put_bound_term(std::move(universally_quantified_variables.top().back()));

    if (scoped_var_count == 1) {
        // If there's only one variable currently scoped, we clear this scope level and reset it.
        universally_quantified_variables.pop();
        universally_quantified_variables.emplace();
    } else
        /*
         * Otherwise, just remove the returned variable. There are other universally quantified variables that need to
         * be returned at this scope level.
         */
        universally_quantified_variables.top().pop_back();
}

void SkolemIntroducingVisitor::eliminate_existential(const IMutableTerm &target_bound_variable)
{
    // Create a new Skolem function to be parameterised by all universally quantified variables in the current scope.
    auto skolem = std::make_unique<MutableSkolemFunction>('S' + std::to_string(skolem_counter++),
        universally_quantified_variables.top());

    /*
     * Indicate to the visitor that all instances of the existentially quantified variable should be replaced by a
     * Skolem function, to be cloned from the templated created above.
     */
    skolem_replacements.emplace(target_bound_variable.get_disambiguated_name(), std::move(skolem));
}

}
