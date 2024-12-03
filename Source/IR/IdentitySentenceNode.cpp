//
// Created by owd on 24/11/24.
//

#include "IdentitySentenceNode.hpp"

#include "../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../Visitors/Sentences/IObservingSentenceVisitor.hpp"

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

void IdentitySentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void IdentitySentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

void IdentitySentenceNode::swap_lhs_operand(std::shared_ptr<ITermNode> new_lhs)
{
    lhs = std::move(new_lhs);
}

void IdentitySentenceNode::swap_rhs_operand(std::shared_ptr<ITermNode> new_rhs)
{
    rhs = std::move(new_rhs);
}

}
