/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the binary-Connected Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "ConnectedSentenceNode.hpp"

#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

ConnectedSentenceNode::ConnectedSentenceNode(const BinaryOperatorTypes operator_type,
        std::unique_ptr<ISentenceNode>&& lhs, std::unique_ptr<ISentenceNode>&& rhs, const bool is_positive) :
    operator_type(operator_type),
    lhs(std::move(lhs)),
    rhs(std::move(rhs)),
    is_positive(is_positive)
{
}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::clone() const
{
    return std::make_unique<ConnectedSentenceNode>(operator_type, lhs->clone(), rhs->clone(), is_positive);
}

void ConnectedSentenceNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool ConnectedSentenceNode::is_negative_polarity() const
{
    return !is_positive;
}

void ConnectedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

BinaryOperatorTypes ConnectedSentenceNode::get_operator_type() const
{
    return operator_type;
}

void ConnectedSentenceNode::set_operator_type(const BinaryOperatorTypes new_type)
{
    operator_type = new_type;
}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::take_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<ISentenceNode> ConnectedSentenceNode::take_rhs_operand()
{
    return std::move(rhs);
}

const ISentenceNode * ConnectedSentenceNode::observe_lhs_operand() const
{
    return lhs.get();
}

const ISentenceNode * ConnectedSentenceNode::observe_rhs_operand() const
{
    return rhs.get();
}

void ConnectedSentenceNode::put_lhs_operand(std::unique_ptr<ISentenceNode> &&operand)
{
    lhs = std::move(operand);
}

void ConnectedSentenceNode::put_rhs_operand(std::unique_ptr<ISentenceNode> &&operand)
{
    rhs = std::move(operand);
}

void ConnectedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t ConnectedSentenceNode::hash() const noexcept
{
    return hash_combine(lhs->hash(), rhs->hash());
}

} // namespace optifol
