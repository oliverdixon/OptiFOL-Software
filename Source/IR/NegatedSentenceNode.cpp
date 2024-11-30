//
// Created by owd on 24/11/24.
//

#include "NegatedSentenceNode.hpp"
#include "../Visitors/Sentences/MutatingVisitorBase.hpp"
#include "../Visitors/Sentences/IObservingVisitor.hpp"

namespace optifol
{

NegatedSentenceNode::NegatedSentenceNode(std::shared_ptr<ISentenceNode> operand) :
        operand(std::move(operand))
{}

std::shared_ptr<ISentenceNode> NegatedSentenceNode::get_operand() const
{
    return operand;
}

void NegatedSentenceNode::accept(MutatingVisitorBase &visitor)
{
    visitor.visit(*this);
}

void NegatedSentenceNode::accept(IObservingVisitor &visitor) const
{
    visitor.visit(*this);
}

}
