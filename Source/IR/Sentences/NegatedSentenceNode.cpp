/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#include "NegatedSentenceNode.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

NegatedSentenceNode::NegatedSentenceNode(std::unique_ptr<ISentenceNode>&& operand) :
        operand(std::move(operand))
{}

std::unique_ptr<ISentenceNode> NegatedSentenceNode::clone() const
{
    return std::make_unique<NegatedSentenceNode>(operand->clone());
}

std::unique_ptr<ISentenceNode> NegatedSentenceNode::take_operand()
{
    return std::move(operand);
}

const ISentenceNode * NegatedSentenceNode::observe_operand() const
{
    return operand.get();
}

void NegatedSentenceNode::put_operand(std::unique_ptr<ISentenceNode> &&operand)
{
    this->operand = std::move(operand);
}

void NegatedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void NegatedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

}
