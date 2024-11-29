//
// Created by owd on 24/11/24.
//

#include "IdentitySentenceNode.hpp"

#include "../Visitors/MutatingVisitorBase.hpp"
#include "../Visitors/IObservingVisitor.hpp"

namespace optifol
{

IdentitySentenceNode::IdentitySentenceNode(std::shared_ptr<ITermNode> lhs, std::shared_ptr<ITermNode> rhs) :
        lhs(std::move(lhs)),
        rhs(std::move(rhs))
{}

std::shared_ptr<ITermNode> IdentitySentenceNode::get_lhs_operand() const
{
    return lhs;
}

std::shared_ptr<ITermNode> IdentitySentenceNode::get_rhs_operand() const
{
    return rhs;
}

void IdentitySentenceNode::accept(MutatingVisitorBase &visitor)
{
    visitor.visit(*this);
}

void IdentitySentenceNode::accept(IObservingVisitor &visitor) const
{
    visitor.visit(*this);
}

}
