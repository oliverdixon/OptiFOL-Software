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

void ConnectedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

BinaryOperatorTypes ConnectedSentenceNode::get_operator_type() const
{
    return operator_type;
}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::get_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::get_rhs_operand()
{
    return std::move(rhs);
}

void ConnectedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

}
