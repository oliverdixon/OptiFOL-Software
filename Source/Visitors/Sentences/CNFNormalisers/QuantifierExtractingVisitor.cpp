/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Quantifier Extracting Visitor
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#include <cassert>

#include "QuantifierExtractingVisitor.hpp"

#include "../../../IR/Mutable/Sentences/MutableConnectedSentenceNode.hpp"
#include "../../../IR/Mutable/Sentences/MutableSentenceRoot.hpp"

namespace optifol
{

const char * QuantifierExtractingVisitor::visitor_name = "QuantifierExtraction";

std::string_view QuantifierExtractingVisitor::get_visitor_name() const
{
    return visitor_name;
}

void QuantifierExtractingVisitor::visit(MutableConnectedSentenceNode &node)
{
    assert(!transformation_metadata.has_value()); // Ensure there is no pending transformation.

    // Recurse down the LHS
    tracking_mode = TrackingMode::LeftMajor;
    auto borrowed_operand_lhs = node.take_lhs_operand();
    borrowed_operand_lhs->accept(*this);

    if (!quant_lhs_data.has_value())
        // If no quantifier, just return the borrowed LHS.
        node.put_lhs_operand(std::move(borrowed_operand_lhs));

    // Recurse down the RHS
    tracking_mode = TrackingMode::RightMajor;
    auto borrowed_operand_rhs = node.take_rhs_operand();
    borrowed_operand_rhs->accept(*this);

    if (!quant_rhs_data.has_value())
        // If no quantifier, just return the borrowed RHS.
        node.put_rhs_operand(std::move(borrowed_operand_rhs));

    tracking_mode = TrackingMode::NotTracking;

    /*
     * Apply the rules:
     *
     *  - If there's a quantifier on the LHS and the RHS, the quantifiers cannot be moved any further outwards.
     *  - If there's neither of the operands were quantifiers, there are no quantifiers to move outwards.
     *  - If there's a quantifier on either the LHS or the RHS, prepare the current MutableConnectedSentenceNode to be the
     *      quantified sentence by retaining the non-quantified side, and using the previously quantified sentence as
     *      the other operand.
     *
     *  At each stage, we ensure that any borrowed sentences/operands have been returned to an owner. Once this
     *  operation is complete, only the bound variable may be held in the quantifier template without a permanent owner.
     */

    if (quant_lhs_data.has_value()) {
        if (!quant_rhs_data.has_value()) {
            // If the LHS only is a quantifier, set our LHS as its sentence.
            node.put_lhs_operand(std::move(quant_lhs_data->sentence));
            transformation_metadata.emplace(quant_lhs_data->type, std::move(quant_lhs_data->bound_term));
        } else {
            // If both operands were quantifiers, there's no transformation to do. Return operands to our node.
            node.put_lhs_operand(std::move(borrowed_operand_lhs));
            node.put_rhs_operand(std::move(borrowed_operand_rhs));

            quant_lhs_data->return_to_owner();
            quant_rhs_data->return_to_owner();

            quant_rhs_data.reset();
        }

        quant_lhs_data.reset();
    } else if (quant_rhs_data.has_value()) {
        // If the RHS only is a quantifier, set our RHS as its sentence.
        node.put_rhs_operand(std::move(quant_rhs_data->sentence));
        transformation_metadata.emplace(quant_rhs_data->type, std::move(quant_rhs_data->bound_term));
        quant_rhs_data.reset();
    }

    // Ensure there are no values with indeterminate owners.
    assert(node.observe_lhs_operand() != nullptr);
    assert(node.observe_rhs_operand() != nullptr);
    assert(!quant_lhs_data.has_value());
    assert(!quant_rhs_data.has_value());
}

void QuantifierExtractingVisitor::visit(MutableQuantifiedSentenceNode &node)
{
    const auto was_tracking = tracking_mode;
    tracking_mode = TrackingMode::NotTracking;

    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);

    /*
     * If there's a pending transformation, as occurs in the case of nested quantified sentence nodes, do the
     * transformation now.
     */
    if (transformation_metadata.has_value()) {
        borrowed_sentence = std::make_unique<MutableQuantifiedSentenceNode>(transformation_metadata->first,
            std::move(transformation_metadata->second), std::move(borrowed_sentence));
        transformation_metadata.reset();

        // The introduction of a new quantified sentence may open new opportunities for reduction, to an arbitrary depth
        borrowed_sentence->accept(*this);
    }

    switch (was_tracking) {
    case TrackingMode::NotTracking:
        node.put_sentence(std::move(borrowed_sentence));
        return;

    case TrackingMode::LeftMajor:
        quant_lhs_data.emplace(node.get_quantifier_type(), node.take_bound_term(), std::move(borrowed_sentence), &node);
        break;

    case TrackingMode::RightMajor:
        quant_rhs_data.emplace(node.get_quantifier_type(), node.take_bound_term(), std::move(borrowed_sentence), &node);
        break;
    }
}

void QuantifierExtractingVisitor::visit(MutableSentenceRoot &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);

    if (transformation_metadata.has_value()) {
        borrowed_sentence = std::make_unique<MutableQuantifiedSentenceNode>(transformation_metadata->first,
            std::move(transformation_metadata->second), std::move(borrowed_sentence));
        transformation_metadata.reset();

        // The introduction of a new quantified sentence may open new opportunities for reduction, to an arbitrary depth
        borrowed_sentence->accept(*this);
    }

    node.put_sentence(std::move(borrowed_sentence));
}

}
