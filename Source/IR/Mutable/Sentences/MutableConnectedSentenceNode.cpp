/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable binary-Connected Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableConnectedSentenceNode.hpp"

#include "../../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../../Visitors/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

MutableConnectedSentenceNode::MutableConnectedSentenceNode(const BinaryOperatorTypes operator_type,
        std::unique_ptr<IMutableSentenceNode>&& lhs, std::unique_ptr<IMutableSentenceNode>&& rhs, const bool is_positive) :
    operator_type(operator_type),
    lhs(std::move(lhs)),
    rhs(std::move(rhs)),
    is_positive(is_positive)
{
}

std::unique_ptr<IMutableSentenceNode> MutableConnectedSentenceNode::clone() const
{
    return std::make_unique<MutableConnectedSentenceNode>(operator_type, lhs->clone(), rhs->clone(), is_positive);
}

void MutableConnectedSentenceNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool MutableConnectedSentenceNode::is_negative_polarity() const
{
    return !is_positive;
}

void MutableConnectedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

BinaryOperatorTypes MutableConnectedSentenceNode::get_operator_type() const
{
    return operator_type;
}

void MutableConnectedSentenceNode::set_operator_type(const BinaryOperatorTypes new_type)
{
    operator_type = new_type;
}

std::unique_ptr<IMutableSentenceNode> MutableConnectedSentenceNode::take_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<IMutableSentenceNode> MutableConnectedSentenceNode::take_rhs_operand()
{
    return std::move(rhs);
}

const IMutableSentenceNode * MutableConnectedSentenceNode::observe_lhs_operand() const
{
    return lhs.get();
}

const IMutableSentenceNode * MutableConnectedSentenceNode::observe_rhs_operand() const
{
    return rhs.get();
}

void MutableConnectedSentenceNode::put_lhs_operand(std::unique_ptr<IMutableSentenceNode> &&operand)
{
    lhs = std::move(operand);
}

void MutableConnectedSentenceNode::put_rhs_operand(std::unique_ptr<IMutableSentenceNode> &&operand)
{
    rhs = std::move(operand);
}

void MutableConnectedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutableConnectedSentenceNode::hash() const noexcept
{
    return hash_polarity(hash_combine_commutative(lhs->hash(), rhs->hash()), is_negative_polarity());
}

std::ostream &MutableConnectedSentenceNode::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(';
    lhs->serialise(ostream);
    ostream << TextSerialiserVisitor::get_operator_symbol(operator_type);
    return rhs->serialise(ostream) << ')';
}

}
