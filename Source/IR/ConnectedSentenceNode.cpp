//
// Created by owd on 24/11/24.
//


#include "ConnectedSentenceNode.hpp"
#include "../Visitors/Sentences/MutatingVisitorBase.hpp"
#include "../Visitors/Sentences/IObservingVisitor.hpp"

namespace optifol
{

ConnectedSentenceNode::ConnectedSentenceNode(BinaryOperatorTypes operator_type, std::shared_ptr<ISentenceNode> lhs,
                                             std::shared_ptr<ISentenceNode> rhs) :
        operator_type(operator_type),
        lhs(std::move(lhs)),
        rhs(std::move(rhs))
{}

void ConnectedSentenceNode::accept(MutatingVisitorBase &visitor)
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

void ConnectedSentenceNode::accept(IObservingVisitor &visitor) const
{
    visitor.visit(*this);
}

}
