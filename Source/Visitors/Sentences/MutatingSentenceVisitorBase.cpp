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

#include "../../IR/Sentences/NodeProxy.hpp"
#include "../../IR/Sentences/NegatedSentenceNode.hpp"
#include "../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../IR/Sentences/QuantifiedSentenceNode.hpp"

namespace optifol
{

void MutatingSentenceVisitorBase::visit(QuantifiedSentenceNode &node)
{
    node.get_sentence()->accept(*this);
}

void MutatingSentenceVisitorBase::visit(ConnectedSentenceNode &node)
{
    node.get_lhs_operand()->accept(*this);
    node.get_rhs_operand()->accept(*this);
}

void MutatingSentenceVisitorBase::visit(NegatedSentenceNode &node)
{
    node.get_operand()->accept(*this);
}

void MutatingSentenceVisitorBase::visit(NodeProxy &node)
{
    node.sentence->accept(*this);
}

void MutatingSentenceVisitorBase::visit(IdentitySentenceNode &node)
{
    (void) node;
}

void MutatingSentenceVisitorBase::visit(PredicationNode &node)
{
    (void) node;
}

}
