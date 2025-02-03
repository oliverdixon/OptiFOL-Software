/*
 * Copyright (c) All Rights Reserved
 * 2024-2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 24/11/24.
//

#include "NegatedSentenceNode.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

NegatedSentenceNode::NegatedSentenceNode(std::shared_ptr<ISentenceNode> operand) :
        operand(std::move(operand))
{}

std::shared_ptr<ISentenceNode> NegatedSentenceNode::get_operand() const
{
    return operand;
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
