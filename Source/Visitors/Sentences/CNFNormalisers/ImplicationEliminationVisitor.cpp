/**
 * @file ImplicationEliminationVisitor.cpp
 * @brief Class implementation for the Implication-Elimination Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-24
 * @version Development
 */

#include "ImplicationEliminationVisitor.hpp"

#include "../../../IR/ConnectedSentenceNode.hpp"
#include "../../../IR/NegatedSentenceNode.hpp"

namespace optifol
{

void ImplicationEliminationVisitor::visit(ConnectedSentenceNode &node)
{
    MutatingSentenceVisitorBase::visit(node);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
    switch (node.get_operator_type()) {
        case BinaryOperatorTypes::Implication:
            node.operator_type = BinaryOperatorTypes::Disjunction;
            node.lhs = std::make_shared<NegatedSentenceNode>(node.lhs);
            break;

        case BinaryOperatorTypes::Biconditional:
            node.operator_type = BinaryOperatorTypes::Conjunction;
            const auto save_lhs = node.lhs;

            node.lhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                               node.lhs,
                                                               std::make_shared<NegatedSentenceNode>(node.rhs));

            node.rhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                               std::make_shared<NegatedSentenceNode>(save_lhs),
                                                               node.rhs);

            break;

    }
#pragma clang diagnostic pop
}

void ImplicationEliminationVisitor::reset()
{
    // Visitor does not maintain any internal state.
    (void)0;
}

}
