//
// Created by owd on 25/11/24.
//

#ifndef OPTIFOL_DISJUNCTIONDISTRIBUTIONVISITOR_HPP
#define OPTIFOL_DISJUNCTIONDISTRIBUTIONVISITOR_HPP

#include <stack>

#include "VisitorBase.hpp"

namespace optifol
{

class DisjunctionDistributionVisitor :
        public VisitorBase
{
public:
    void visit(ConnectedSentenceNode &node) override
    {
        const auto current_operator_type = node.get_operator_type();

        if (current_operator_type == BinaryOperatorTypes::Conjunction)
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
            tracking_state = TrackingState::NotTracking;

        if (current_operator_type == BinaryOperatorTypes::Disjunction) {
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

private:
    enum class TrackingState
    {
        NotTracking,
        LeftMajor,
        RightMajor
    };

    TrackingState tracking_state = TrackingState::NotTracking;
    std::stack<std::pair<std::shared_ptr<ISentenceNode>, std::shared_ptr<ISentenceNode>>> tracked_operands;

    bool attempt_reduction(ConnectedSentenceNode &node)
    {
        if (!tracked_operands.empty()) {
            const auto simple = (tracking_state == TrackingState::LeftMajor) ? node.rhs : node.lhs;
            auto& complex = tracked_operands.top();

            node.operator_type = BinaryOperatorTypes::Conjunction;
            node.lhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction, simple,
                                                               std::move(complex.first));
            node.rhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction, simple,
                                                               std::move(complex.second));

            tracked_operands.pop();
            return true;
        }

        return false;
    }
};

}

#endif //OPTIFOL_DISJUNCTIONDISTRIBUTIONVISITOR_HPP
