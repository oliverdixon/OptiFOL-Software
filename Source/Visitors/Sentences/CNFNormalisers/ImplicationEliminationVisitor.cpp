/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Implication-Elimination Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-24
 * @version Development
 */

#include "ImplicationEliminationVisitor.hpp"

#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/NegatedSentenceNode.hpp"

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
        node.lhs = std::make_unique<NegatedSentenceNode>(node.take_lhs_operand());
        break;

    case BinaryOperatorTypes::Biconditional:
        node.operator_type = BinaryOperatorTypes::Conjunction;

        auto save_lhs = node.take_lhs_operand();
        auto save_rhs = node.take_rhs_operand();

        node.lhs = std::make_unique<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                           save_lhs->clone(),
                                                           std::make_unique<NegatedSentenceNode>(save_rhs->clone()));
        node.rhs = std::make_unique<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                           std::make_unique<NegatedSentenceNode>(std::move(save_lhs)),
                                                           std::move(save_rhs));
        break;
    }
#pragma clang diagnostic pop
}

void ImplicationEliminationVisitor::reset()
{
    // Visitor does not maintain any internal state.
    (void) 0;
}
}
