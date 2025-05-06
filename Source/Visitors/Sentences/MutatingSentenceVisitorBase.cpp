/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Mutating Sentence Visitor base class.
 * @author Oliver Dixon
 * @date 2024-11-24
 * @version Development
 */

#include "MutatingSentenceVisitorBase.hpp"

#include "../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../IR/Sentences/QuantifiedSentenceNode.hpp"
#include "../../IR/Sentences/SentenceRoot.hpp"

namespace optifol
{

void MutatingSentenceVisitorBase::visit(QuantifiedSentenceNode &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    node.put_sentence(std::move(borrowed_sentence));
}

void MutatingSentenceVisitorBase::visit(ConnectedSentenceNode &node)
{
    auto borrowed_operand = node.take_lhs_operand();
    borrowed_operand->accept(*this);
    node.put_lhs_operand(std::move(borrowed_operand));

    borrowed_operand = node.take_rhs_operand();
    borrowed_operand->accept(*this);
    node.put_rhs_operand(std::move(borrowed_operand));
}

void MutatingSentenceVisitorBase::visit(IdentitySentenceNode &node)
{
    (void) node;
}

void MutatingSentenceVisitorBase::visit(PredicationNode &node)
{
    (void) node;
}

void MutatingSentenceVisitorBase::visit(SentenceRoot &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    node.put_sentence(std::move(borrowed_sentence));
}

}
