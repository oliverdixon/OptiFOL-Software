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
        // If we're a conjunction node, we might be a candidate child. Check if we should be tracking.
        switch (tracking_state) {
        case TrackingState::LeftMajor:
            tracked_operands.emplace(node.take_lhs_operand(), node.take_rhs_operand());
            break;

        case TrackingState::RightMajor:
            tracked_operands.emplace(node.take_rhs_operand(), node.take_lhs_operand());
            break;

        case TrackingState::NotTracking:
            break;
        }
    else
        // If we're not a child candidate, we mark the end of any chain. Stop the tracking here.
        tracking_state = TrackingState::NotTracking;

    if (current_operator_type == BinaryOperatorTypes::Disjunction) {
        /* If we're a disjunction node, we might be a candidate parent. Check the left and right branches for
         * candidate children, reducing if necessary. Once we've done a reduction, stop the tracking. */

        tracking_state = TrackingState::RightMajor;
        node.take_rhs_operand()->accept(*this);

        if (!attempt_reduction(node)) {
            tracking_state = TrackingState::LeftMajor;
            node.take_lhs_operand()->accept(*this);
            attempt_reduction(node);
        }

        tracking_state = TrackingState::NotTracking;
    }
}

bool DisjunctionDistributionVisitor::attempt_reduction(ConnectedSentenceNode &node)
{
#if 0
    assert(node.get_operator_type() == BinaryOperatorTypes::Disjunction);

    if (!tracked_operands.empty()) {
        auto simple_lhs = tracking_state == TrackingState::LeftMajor ? std::move(node.rhs) :
            std::move(node.lhs);
        std::unique_ptr<ISentenceNode> simple_rhs(&*simple_lhs);
        auto &complex = tracked_operands.top();

        assert(complex.first != nullptr && complex.second != nullptr);

        node.operator_type = BinaryOperatorTypes::Conjunction;
        node.lhs = std::make_unique<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction, std::move(simple_lhs),
                                                           std::move(complex.first));
        node.rhs = std::make_unique<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction, std::move(simple_rhs),
                                                           std::move(complex.second));

        tracked_operands.pop();
        MutatingSentenceVisitorBase::visit(node);

        assert(tracked_operands.empty());
        return true;
    }

    return false;
#endif

    // TODO
    return false;
}

void DisjunctionDistributionVisitor::reset()
{
    tracking_state = TrackingState::NotTracking;

    while (!tracked_operands.empty())
        tracked_operands.pop();
}

}
