/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Identity Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "IdentitySentenceNode.hpp"

#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

IdentitySentenceNode::IdentitySentenceNode(std::unique_ptr<ITermNode>&& lhs, std::unique_ptr<ITermNode>&& rhs,
        const bool is_positive) :
    lhs(std::move(lhs)),
    rhs(std::move(rhs)),
    is_positive(is_positive)
{}

std::unique_ptr<ISentenceNode> IdentitySentenceNode::clone() const
{
    return std::make_unique<IdentitySentenceNode>(lhs->clone(), rhs->clone(), is_positive);
}

void IdentitySentenceNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool IdentitySentenceNode::is_negative_polarity() const
{
    return !is_positive;
}

std::unique_ptr<ITermNode> IdentitySentenceNode::take_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<ITermNode> IdentitySentenceNode::take_rhs_operand()
{
    return std::move(rhs);
}

const ITermNode * IdentitySentenceNode::observe_lhs_operand() const
{
    return lhs.get();
}

const ITermNode * IdentitySentenceNode::observe_rhs_operand() const
{
    return rhs.get();
}

void IdentitySentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void IdentitySentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

void IdentitySentenceNode::swap_lhs_operand(std::unique_ptr<ITermNode>&& new_lhs)
{
    lhs.swap(new_lhs);
}

void IdentitySentenceNode::swap_rhs_operand(std::unique_ptr<ITermNode>&& new_rhs)
{
    rhs.swap(new_rhs);
}

void IdentitySentenceNode::put_lhs_operand(std::unique_ptr<ITermNode> &&new_lhs)
{
    lhs = std::move(new_lhs);
}

void IdentitySentenceNode::put_rhs_operand(std::unique_ptr<ITermNode> &&new_rhs)
{
    rhs = std::move(new_rhs);
}

}
