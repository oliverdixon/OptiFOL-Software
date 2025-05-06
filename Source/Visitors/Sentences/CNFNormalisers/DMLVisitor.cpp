/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the De Morgan's Laws Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-27
 * @version Development
 */

#include "DMLVisitor.hpp"
#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"

namespace optifol
{

void DMLVisitor::visit(ConnectedSentenceNode &node)
{
    /*
     * If we're in an immediately negative context ~(ConnectedSentenceNode), we apply the transformation according to
     * the binary operator type. This requires taking temporary ownership of the operands.
     */
    if (node.is_negative_polarity()) {
        auto borrowed_lhs = node.take_lhs_operand();
        auto borrowed_rhs = node.take_rhs_operand();

        switch (node.get_operator_type()) {
        case BinaryOperatorTypes::Disjunction:
            node.set_operator_type(BinaryOperatorTypes::Conjunction);
            borrowed_lhs->flip_polarity();
            borrowed_rhs->flip_polarity();
            node.flip_polarity();
            break;

        case BinaryOperatorTypes::Conjunction:
            node.set_operator_type(BinaryOperatorTypes::Disjunction);
            borrowed_lhs->flip_polarity();
            borrowed_rhs->flip_polarity();
            node.flip_polarity();
            break;

        default:
            (void) 0;
        }

        node.put_lhs_operand(std::move(borrowed_lhs));
        node.put_rhs_operand(std::move(borrowed_rhs));
    }

    /*
     * Regardless of whether we did a transformation, there may be further opportunities for transformation on the
     * individual operands.
     */
    MutatingSentenceVisitorBase::visit(node);
}

void DMLVisitor::visit(QuantifiedSentenceNode &node)
{
    /*
     * If we're in an immediately negative context ~(QuantifiedSentenceNode), we apply the transformation according to
     * the quantification type. This requires taking temporary ownership of the quantified sentence.
     */
    if (node.is_negative_polarity()) {
        auto borrowed_sentence = node.take_sentence();

        switch (node.get_quantifier_type()) {
        case QuantifierTypes::Universal:
            node.set_quantifier_type(QuantifierTypes::Existential);
            borrowed_sentence->flip_polarity();
            break;

        case QuantifierTypes::Existential:
            node.set_quantifier_type(QuantifierTypes::Universal);
            borrowed_sentence->flip_polarity();
            break;
        }

        node.put_sentence(std::move(borrowed_sentence));
        node.flip_polarity();
    }

    /*
     * Regardless of whether we did a transformation, there may be further opportunities for transformation on the
     * quantified sentence.
     */
    MutatingSentenceVisitorBase::visit(node);
}

}
