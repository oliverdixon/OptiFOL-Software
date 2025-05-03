/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#include "IdentitySentenceNode.hpp"

#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

IdentitySentenceNode::IdentitySentenceNode(std::unique_ptr<ITermNode>&& lhs, std::unique_ptr<ITermNode>&& rhs) :
        lhs(std::move(lhs)),
        rhs(std::move(rhs))
{}

std::unique_ptr<ITermNode> IdentitySentenceNode::get_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<ITermNode> IdentitySentenceNode::get_rhs_operand()
{
    return std::move(rhs);
}

void IdentitySentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void IdentitySentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

void IdentitySentenceNode::swap_lhs_operand(std::unique_ptr<ITermNode>& new_lhs)
{
    lhs.swap(new_lhs);
}

void IdentitySentenceNode::swap_rhs_operand(std::unique_ptr<ITermNode>& new_rhs)
{
    rhs.swap(new_rhs);
}

}
