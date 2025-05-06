/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Disjunction-Distribution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-25
 * @version Development
 */

#include <cassert>

#include "DisjunctionDistributionVisitor.hpp"
#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"

namespace optifol
{

void DisjunctionDistributionVisitor::visit(ConnectedSentenceNode &node)
{
    const auto current_operator_type = node.get_operator_type();

    if (current_operator_type == BinaryOperatorTypes::Conjunction)
        /*
         * If we're a conjunction node, we might be a candidate child. Transfer ownership of our LHS and RHS operands to
         * the top layer of the operand stack, with the order determined by the tracking mode, to be returned by the
         * attempt_reduction member function.
         */
        switch (tracking_mode) {
        case TrackingMode::LeftMajor:
            tracked_operands.emplace(node.take_lhs_operand(), node.take_rhs_operand());
            break;

        case TrackingMode::RightMajor:
            tracked_operands.emplace(node.take_rhs_operand(), node.take_lhs_operand());
            break;

        case TrackingMode::NotTracking:
            break;
        }
    else
        // If we're not a child candidate, we mark the end of any chain. Stop the tracking here.
        tracking_mode = TrackingMode::NotTracking;

    if (current_operator_type == BinaryOperatorTypes::Disjunction) {
        /*
         * If we're a disjunction node, we might be a candidate parent. Check the left and right branches for
         * candidate children, reducing recursively where necessary. Once we've reduced on both branches, and ownership
         * has been returned, stop the tracking.
         */

        tracking_mode = TrackingMode::RightMajor;
        auto borrowed_rhs = node.take_rhs_operand();
        borrowed_rhs->accept(*this);
        node.put_rhs_operand(std::move(borrowed_rhs));

        if (!attempt_reduction(node)) {
            // If we can't do a reduction on the RHS, try the LHS.

            tracking_mode = TrackingMode::LeftMajor;
            auto borrowed_lhs = node.take_lhs_operand();
            borrowed_lhs->accept(*this);
            node.put_lhs_operand(std::move(borrowed_lhs));

            attempt_reduction(node);
        }

        tracking_mode = TrackingMode::NotTracking;
    }
}

bool DisjunctionDistributionVisitor::attempt_reduction(ConnectedSentenceNode &node)
{
    // Candidate children are, by definition, disjunctive clauses.
    assert(node.get_operator_type() == BinaryOperatorTypes::Disjunction);

    if (!tracked_operands.empty()) {
        /*
         * If the tracked operands stack is non-empty, it still holds ownership of operands in clauses that need to be
         * distributed. We consider four 'destination' operands based on the top layer of the stack used to construct
         * the distributed conjunctive clause of disjuncts:
         *
         *  - LHS/LHS: The LHS operand of the first disjunct
         *  - LHS/RHS: The RHS operand of the first disjunct
         *  - RHS/LHS: The LHS operand of the second disjunct. This is the distributed literal cloned from LHS/LHS.
         *  - RHS/RHS: The RHS operand of the second disjunct.
         */

        auto destination_lhs_lhs = tracking_mode == TrackingMode::LeftMajor ?
            node.take_rhs_operand() : node.take_lhs_operand();
        auto destination_lhs_rhs = std::move(tracked_operands.top().first);

        auto destination_rhs_lhs = destination_lhs_lhs->clone();
        auto destination_rhs_rhs = std::move(tracked_operands.top().second);

        node.set_operator_type(BinaryOperatorTypes::Conjunction);

        node.put_lhs_operand(
            std::make_unique<ConnectedSentenceNode>(
                BinaryOperatorTypes::Disjunction,
                std::move(destination_lhs_lhs),
                std::move(destination_lhs_rhs)
            )
        );

        node.put_rhs_operand(
            std::make_unique<ConnectedSentenceNode>(
                BinaryOperatorTypes::Disjunction,
                std::move(destination_rhs_lhs),
                std::move(destination_rhs_rhs)
            )
        );

        tracked_operands.pop();
        MutatingSentenceVisitorBase::visit(node);

        /*
         * The above recursive call should empty the tracked operands stack with this member function. If we end with a
         * non-empty stack, it still owns operands that should've been returned to the ConnectedSentenceNode or used to
         * construct a new operand.
         */
        assert(tracked_operands.empty());
        return true;
    }

    return false;
}

void DisjunctionDistributionVisitor::reset()
{
    tracking_mode = TrackingMode::NotTracking;

    while (!tracked_operands.empty())
        tracked_operands.pop();
}

}
