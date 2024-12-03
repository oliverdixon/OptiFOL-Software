/**
 * @file DisjunctionDistributionVisitor.cpp
 * @brief Class implementation for the Disjunction-Distribution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-25
 * @version Development
 */

#include <cassert>

#include "DisjunctionDistributionVisitor.hpp"
#include "../../../IR/ConnectedSentenceNode.hpp"

namespace optifol
{

void DisjunctionDistributionVisitor::visit(ConnectedSentenceNode &node)
{
    const auto current_operator_type = node.get_operator_type();

    if (current_operator_type == BinaryOperatorTypes::Conjunction)
        // If we're a conjunction node, we might be a candidate child. Check if we should be tracking.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
        switch (tracking_state) {
            case TrackingState::LeftMajor:
                tracked_operands.emplace(node.get_lhs_operand(), node.get_rhs_operand());
                break;

            case TrackingState::RightMajor:
                tracked_operands.emplace(node.get_rhs_operand(), node.get_lhs_operand());
                break;
        }
#pragma clang diagnostic pop
    else
        // If we're not a child candidate, we mark the end of any chain. Stop the tracking here.
        tracking_state = TrackingState::NotTracking;

    if (current_operator_type == BinaryOperatorTypes::Disjunction) {
        /* If we're a disjunction node, we might be a candidate parent. Check the left and right branches for
         * candidate children, reducing if necessary. Once we've done a reduction, stop the tracking. */

        tracking_state = TrackingState::RightMajor;
        node.get_rhs_operand()->accept(*this);

        if (!attempt_reduction(node)) {
            tracking_state = TrackingState::LeftMajor;
            node.get_lhs_operand()->accept(*this);
            attempt_reduction(node);
        }

        tracking_state = TrackingState::NotTracking;
    }
}

bool DisjunctionDistributionVisitor::attempt_reduction(ConnectedSentenceNode &node)
{
    assert(node.get_operator_type() == BinaryOperatorTypes::Disjunction);

    if (!tracked_operands.empty()) {
        const auto simple = (tracking_state == TrackingState::LeftMajor) ? node.rhs : node.lhs;
        auto &complex = tracked_operands.top();

        assert(complex.first != nullptr && complex.second != nullptr);

        node.operator_type = BinaryOperatorTypes::Conjunction;
        node.lhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction, simple,
                                                           std::move(complex.first));
        node.rhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction, simple,
                                                           std::move(complex.second));

        tracked_operands.pop();
        MutatingSentenceVisitorBase::visit(node);

        assert(tracked_operands.empty());
        return true;
    }

    return false;
}

void DisjunctionDistributionVisitor::reset()
{
    tracking_state = TrackingState::NotTracking;

    while (!tracked_operands.empty())
        tracked_operands.pop();
}

}
