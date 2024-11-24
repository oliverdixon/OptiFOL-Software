//
// Created by owd on 24/11/24.
//

#include "NegatedSentenceNode.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

NegatedSentenceNode::NegatedSentenceNode(std::shared_ptr<ISentenceNode> operand) :
        operand(std::move(operand))
{}

std::string NegatedSentenceNode::to_string() const
{
    return "~ " + operand->to_string();
}

std::shared_ptr<ISentenceNode> NegatedSentenceNode::get_operand() const
{
    return operand;
}

void NegatedSentenceNode::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

}
