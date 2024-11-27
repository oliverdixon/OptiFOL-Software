//
// Created by owd on 24/11/24.
//


#include "ConnectedSentenceNode.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

ConnectedSentenceNode::ConnectedSentenceNode(BinaryOperatorTypes operator_type, std::shared_ptr<ISentenceNode> lhs,
                                             std::shared_ptr<ISentenceNode> rhs) :
        operator_type(operator_type),
        lhs(std::move(lhs)),
        rhs(std::move(rhs))
{}

std::string ConnectedSentenceNode::to_string() const
{
    return "(" + lhs->to_string() + ' ' + operator_symbol() + ' ' + rhs->to_string() + ')';
}

void ConnectedSentenceNode::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

BinaryOperatorTypes ConnectedSentenceNode::get_operator_type() const
{
    return operator_type;
}

std::shared_ptr<ISentenceNode> ConnectedSentenceNode::get_lhs_operand() const
{
    return lhs;
}

std::shared_ptr<ISentenceNode> ConnectedSentenceNode::get_rhs_operand() const
{
    return rhs;
}

}
