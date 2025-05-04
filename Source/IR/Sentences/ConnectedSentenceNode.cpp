/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */




#include "ConnectedSentenceNode.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

ConnectedSentenceNode::ConnectedSentenceNode(const BinaryOperatorTypes operator_type,
    std::unique_ptr<ISentenceNode>&& lhs, std::unique_ptr<ISentenceNode>&& rhs) :
        operator_type(operator_type),
        lhs(std::move(lhs)),
        rhs(std::move(rhs))
{}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::clone() const
{
    return std::make_unique<ConnectedSentenceNode>(operator_type, lhs->clone(), rhs->clone());
}

void ConnectedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

BinaryOperatorTypes ConnectedSentenceNode::get_operator_type() const
{
    return operator_type;
}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::take_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::take_rhs_operand()
{
    return std::move(rhs);
}

const ISentenceNode * ConnectedSentenceNode::observe_lhs_operand() const
{
    return lhs.get();
}

const ISentenceNode * ConnectedSentenceNode::observe_rhs_operand() const
{
    return rhs.get();
}

void ConnectedSentenceNode::put_lhs_operand(std::unique_ptr<ISentenceNode> &&operand)
{
    lhs = std::move(operand);
}

void ConnectedSentenceNode::put_rhs_operand(std::unique_ptr<ISentenceNode> &&operand)
{
    rhs = std::move(operand);
}

void ConnectedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

}
