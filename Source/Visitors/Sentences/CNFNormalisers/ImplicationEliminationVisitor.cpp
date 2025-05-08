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

namespace optifol
{

void ImplicationEliminationVisitor::visit(ConnectedSentenceNode &node)
{
    MutatingSentenceVisitorBase::visit(node);

    const auto operator_type = node.get_operator_type();

    if (operator_type == BinaryOperatorTypes::Implication) {
        node.set_operator_type(BinaryOperatorTypes::Disjunction);
        auto borrowed_lhs = node.take_lhs_operand();
        borrowed_lhs->flip_polarity();
        node.put_lhs_operand(std::move(borrowed_lhs));
    } else if (operator_type == BinaryOperatorTypes::Biconditional) {
        node.set_operator_type(BinaryOperatorTypes::Conjunction);

        auto save_lhs = node.take_lhs_operand();
        auto save_rhs = node.take_rhs_operand();

        auto new_rhs = save_rhs->clone();
        new_rhs->flip_polarity();

        node.put_lhs_operand(std::make_unique<ConnectedSentenceNode>(
            BinaryOperatorTypes::Disjunction,
            save_lhs->clone(),
            std::move(new_rhs)
        ));

        save_lhs->flip_polarity();

        node.put_rhs_operand(std::make_unique<ConnectedSentenceNode>(
            BinaryOperatorTypes::Disjunction,
            std::move(save_lhs),
            std::move(save_rhs)
        ));
    }
}

}
