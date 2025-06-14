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

#include "MutableBinaryConnected.hpp"

#include "../../../Visitors/MutableTargets/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../../Visitors/MutableTargets/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

MutableBinaryConnected::MutableBinaryConnected(const BinaryOperatorTypes operator_type,
        std::unique_ptr<IMutableSentence>&& lhs, std::unique_ptr<IMutableSentence>&& rhs, const bool is_positive) :
    operator_type(operator_type),
    lhs(std::move(lhs)),
    rhs(std::move(rhs)),
    is_positive(is_positive)
{
}

std::unique_ptr<IMutableSentence> MutableBinaryConnected::clone() const
{
    return std::make_unique<MutableBinaryConnected>(operator_type, lhs->clone(), rhs->clone(), is_positive);
}

void MutableBinaryConnected::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool MutableBinaryConnected::is_negative_polarity() const noexcept
{
    return !is_positive;
}

void MutableBinaryConnected::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

BinaryOperatorTypes MutableBinaryConnected::get_operator_type() const
{
    return operator_type;
}

void MutableBinaryConnected::set_operator_type(const BinaryOperatorTypes new_type)
{
    operator_type = new_type;
}

std::unique_ptr<IMutableSentence> MutableBinaryConnected::take_lhs_operand()
{
    return std::move(lhs);
}

std::unique_ptr<IMutableSentence> MutableBinaryConnected::take_rhs_operand()
{
    return std::move(rhs);
}

const IMutableSentence * MutableBinaryConnected::observe_lhs_operand() const
{
    return lhs.get();
}

const IMutableSentence * MutableBinaryConnected::observe_rhs_operand() const
{
    return rhs.get();
}

void MutableBinaryConnected::put_lhs_operand(std::unique_ptr<IMutableSentence> &&operand)
{
    lhs = std::move(operand);
}

void MutableBinaryConnected::put_rhs_operand(std::unique_ptr<IMutableSentence> &&operand)
{
    rhs = std::move(operand);
}

void MutableBinaryConnected::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutableBinaryConnected::hash() const noexcept
{
    return hash_polarity(hash_combine_commutative(lhs->hash(), rhs->hash()), is_negative_polarity());
}

std::ostream &MutableBinaryConnected::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(';
    lhs->serialise(ostream);
    ostream << TextSerialiserVisitor::get_operator_symbol(operator_type);
    return rhs->serialise(ostream) << ')';
}

}
