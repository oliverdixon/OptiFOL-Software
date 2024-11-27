//
// Created by owd on 24/11/24.
//

#include "IdentitySentenceNode.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

IdentitySentenceNode::IdentitySentenceNode(std::shared_ptr<ITermNode> lhs, std::shared_ptr<ITermNode> rhs) :
        lhs(std::move(lhs)),
        rhs(std::move(rhs))
{}

std::string IdentitySentenceNode::to_string() const
{
    return lhs->to_string() + " = " + rhs->to_string();
}

std::shared_ptr<ITermNode> IdentitySentenceNode::get_lhs_operand() const
{
    return lhs;
}

std::shared_ptr<ITermNode> IdentitySentenceNode::get_rhs_operand() const
{
    return rhs;
}

void IdentitySentenceNode::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

}
