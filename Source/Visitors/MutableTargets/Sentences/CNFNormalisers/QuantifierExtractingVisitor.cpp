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

#include "../../../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"

namespace optifol
{

const char *QuantifierExtractingVisitor::visitor_name = "QuantifierExtraction";

std::string_view QuantifierExtractingVisitor::get_visitor_name() const
{
    return visitor_name;
}

void QuantifierExtractingVisitor::visit(MutableBinaryConnected &node)
{
    assert(!transformation.has_value()); // Ensure there is no pending transformation.

    // Recurse down the LHS and apply any applicable transforms generated during the visit.
    tracking_mode = TrackingMode::LeftMajor;
    auto borrowed_operand_lhs = node.take_lhs_operand();
    borrowed_operand_lhs->accept(*this);
    borrowed_operand_lhs = apply_transform(std::move(borrowed_operand_lhs));

    if (!quant_lhs_data.has_value())
        // If no quantifier, just return the borrowed LHS.
        node.put_lhs_operand(std::move(borrowed_operand_lhs));

    // Recurse down the RHS and apply any applicable transforms generated during the visit.
    tracking_mode = TrackingMode::RightMajor;
    auto borrowed_operand_rhs = node.take_rhs_operand();
    borrowed_operand_rhs->accept(*this);
    borrowed_operand_rhs = apply_transform(std::move(borrowed_operand_rhs));

    if (!quant_rhs_data.has_value())
        // If no quantifier, just return the borrowed RHS.
        node.put_rhs_operand(std::move(borrowed_operand_rhs));

    assert(!transformation.has_value()); // Ensure there is still no pending transformation.
    tracking_mode = TrackingMode::NotTracking;

    /*
     * Apply the rules:
     *
     *  - If there's a quantifier on the LHS and the RHS, the quantifiers cannot be moved any further outwards.
     *  - If there's neither of the operands were quantifiers, there are no quantifiers to move outwards.
     *  - If there's a quantifier on either the LHS or the RHS, prepare the current MutableBinaryConnected to be the
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
            transformation.emplace(quant_lhs_data->type, std::move(quant_lhs_data->bound_term));
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
        transformation.emplace(quant_rhs_data->type, std::move(quant_rhs_data->bound_term));
        quant_rhs_data.reset();
    }

    // Ensure there are no values with indeterminate owners.
    assert(node.observe_lhs_operand() != nullptr);
    assert(node.observe_rhs_operand() != nullptr);
    assert(!quant_lhs_data.has_value());
    assert(!quant_rhs_data.has_value());
}

void QuantifierExtractingVisitor::visit(MutableQuantified &node)
{
    const auto was_tracking = tracking_mode;
    tracking_mode = TrackingMode::NotTracking;

    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    borrowed_sentence = apply_transform(std::move(borrowed_sentence));

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
    node.put_sentence(apply_transform(std::move(borrowed_sentence)));
}

std::unique_ptr<IMutableSentence> QuantifierExtractingVisitor::apply_transform(
        std::unique_ptr<IMutableSentence> &&transform_target)
{
    if (transformation.has_value()) {
        transform_target = std::make_unique<MutableQuantified>(
                transformation->first, std::move(transformation->second), std::move(transform_target));
        transformation.reset();

        // The introduction of a new quantified sentence may open new opportunities for reduction, to an arbitrary depth
        transform_target->accept(*this);
    }

    return transform_target;
}

} // namespace optifol
