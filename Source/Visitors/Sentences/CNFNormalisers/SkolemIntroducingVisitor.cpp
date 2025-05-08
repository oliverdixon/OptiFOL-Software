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

#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/IdentitySentenceNode.hpp"
#include "../../../IR/Sentences/PredicationNode.hpp"
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"
#include "../../../IR/Sentences/SentenceRoot.hpp"
#include "../../../IR/Terms/SkolemFunctionNode.hpp"

namespace optifol
{

SkolemIntroducingVisitor::SkolemIntroducingVisitor()
{
    universally_quantified_variables.emplace();
}

void SkolemIntroducingVisitor::visit(QuantifiedSentenceNode &node)
{
    auto borrowed_sentence = node.take_sentence();

    switch (node.get_quantifier_type()) {
    case QuantifierTypes::Universal:
        open_scope(node.observe_bound_term()->clone());
        borrowed_sentence->accept(*this);
        close_latest_scope();

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

void SkolemIntroducingVisitor::visit(PredicationNode &node)
{
    if (skolem_replacements.empty())
        MutatingSentenceVisitorBase::visit(node);

    auto &args = node.arguments;
    const auto argument_count = args.size();

    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto& potential_replacement =
            skolem_replacements.find(args[i]->get_disambiguated_name());
        if (potential_replacement != skolem_replacements.cend())
            args[i] = potential_replacement->second->clone();

        args[i]->accept(term_visitor);
    }
}

void SkolemIntroducingVisitor::visit(IdentitySentenceNode &node)
{
    auto borrowed_lhs = node.take_lhs_operand();

    // Apply any relevant disambiguation rewriting to the LHS operand.
    const auto& lhs_rule =
        skolem_replacements.find(borrowed_lhs->get_disambiguated_name());
    if (lhs_rule != skolem_replacements.cend())
        node.swap_lhs_operand(lhs_rule->second->clone());

    borrowed_lhs->accept(term_visitor);
    node.put_lhs_operand(std::move(borrowed_lhs));

    auto borrowed_rhs = node.take_rhs_operand();

    // Apply any relevant disambiguation rewriting to the RHS operand.
    const auto& rhs_rule =
        skolem_replacements.find(borrowed_rhs->get_disambiguated_name());
    if (rhs_rule != skolem_replacements.cend())
        node.swap_rhs_operand(rhs_rule->second->clone());

    borrowed_rhs->accept(term_visitor);
    node.put_rhs_operand(std::move(borrowed_rhs));
}

void SkolemIntroducingVisitor::visit(ConnectedSentenceNode &node)
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

void SkolemIntroducingVisitor::visit(SentenceRoot &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    if (extracted_sentence == nullptr)
        node.put_sentence(std::move(borrowed_sentence));
    else
        node.put_sentence(std::move(extracted_sentence));
}

void SkolemIntroducingVisitor::open_scope(std::unique_ptr<ITermNode> &&cloned_bound_variable)
{
    universally_quantified_variables.top().push_back(std::move(cloned_bound_variable));
}

void SkolemIntroducingVisitor::close_latest_scope()
{
    universally_quantified_variables.pop();
    universally_quantified_variables.emplace();
}

void SkolemIntroducingVisitor::eliminate_existential(const ITermNode &target_bound_variable)
{
    // Create a new Skolem function to be parameterised by all universally quantified variables in the current scope.
    auto skolem = std::make_unique<SkolemFunctionNode>('S' + std::to_string(skolem_counter++),
        universally_quantified_variables.top());

    /*
     * Indicate to the visitor that all instances of the existentially quantified variable should be replaced by a
     * Skolem function, to be cloned from the templated created above.
     */
    skolem_replacements.emplace(target_bound_variable.get_disambiguated_name(), std::move(skolem));
}

}
