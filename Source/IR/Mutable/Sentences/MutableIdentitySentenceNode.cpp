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

#include "MutableIdentitySentenceNode.hpp"

#include "../../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

MutableIdentitySentenceNode::MutableIdentitySentenceNode(std::unique_ptr<IMutableTermNode>&& lhs, std::unique_ptr<IMutableTermNode>&& rhs,
        const bool is_positive) :
    lhs(std::move(lhs)),
    rhs(std::move(rhs)),
    is_positive(is_positive)
{}

std::unique_ptr<IMutableSentenceNode> MutableIdentitySentenceNode::clone() const
{
    return std::make_unique<MutableIdentitySentenceNode>(lhs->clone(), rhs->clone(), is_positive);
}

void MutableIdentitySentenceNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool MutableIdentitySentenceNode::is_negative_polarity() const
{
    return !is_positive;
}

std::unique_ptr<IMutableTermNode> MutableIdentitySentenceNode::take_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<IMutableTermNode> MutableIdentitySentenceNode::take_rhs_operand()
{
    return std::move(rhs);
}

const IMutableTermNode * MutableIdentitySentenceNode::observe_lhs_operand() const
{
    return lhs.get();
}

const IMutableTermNode * MutableIdentitySentenceNode::observe_rhs_operand() const
{
    return rhs.get();
}

void MutableIdentitySentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutableIdentitySentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutableIdentitySentenceNode::hash() const noexcept
{
    return hash_polarity(hash_combine_commutative(lhs->hash(), rhs->hash()), is_negative_polarity());
}

std::ostream &MutableIdentitySentenceNode::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(';
    lhs->serialise(ostream);
    ostream << " == ";
    return rhs->serialise(ostream) << ')';
}

void MutableIdentitySentenceNode::swap_lhs_operand(std::unique_ptr<IMutableTermNode>&& new_lhs)
{
    lhs.swap(new_lhs);
}

void MutableIdentitySentenceNode::swap_rhs_operand(std::unique_ptr<IMutableTermNode>&& new_rhs)
{
    rhs.swap(new_rhs);
}

void MutableIdentitySentenceNode::put_lhs_operand(std::unique_ptr<IMutableTermNode> &&new_lhs)
{
    lhs = std::move(new_lhs);
}

void MutableIdentitySentenceNode::put_rhs_operand(std::unique_ptr<IMutableTermNode> &&new_rhs)
{
    rhs = std::move(new_rhs);
}

}
