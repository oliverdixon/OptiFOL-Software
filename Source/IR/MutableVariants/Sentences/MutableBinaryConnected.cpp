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

namespace optifol
{

MutableBinaryConnected::MutableBinaryConnected(const BinaryOperatorTypes operator_type,
        std::unique_ptr<IMutableSentence> &&lhs, std::unique_ptr<IMutableSentence> &&rhs, const bool is_positive) :
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

BinaryOperatorTypes MutableBinaryConnected::get_operator_type() const noexcept
{
    return operator_type;
}

void MutableBinaryConnected::set_operator_type(const BinaryOperatorTypes new_type) noexcept
{
    operator_type = new_type;
}

std::unique_ptr<IMutableSentence> MutableBinaryConnected::take_lhs_operand() noexcept
{
    return std::move(lhs);
}

std::unique_ptr<IMutableSentence> MutableBinaryConnected::take_rhs_operand() noexcept
{
    return std::move(rhs);
}

const IMutableSentence *MutableBinaryConnected::observe_lhs_operand() const noexcept
{
    return lhs.get();
}

const IMutableSentence *MutableBinaryConnected::observe_rhs_operand() const noexcept
{
    return rhs.get();
}

void MutableBinaryConnected::put_lhs_operand(std::unique_ptr<IMutableSentence> &&operand) noexcept
{
    lhs = std::move(operand);
}

void MutableBinaryConnected::put_rhs_operand(std::unique_ptr<IMutableSentence> &&operand) noexcept
{
    rhs = std::move(operand);
}

void MutableBinaryConnected::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutableBinaryConnected::hash() const noexcept
{
    return BinaryConnected::hash_binary_connected(operator_type, lhs.get(), rhs.get(), is_negative_polarity());
}

std::ostream &MutableBinaryConnected::serialise(std::ostream &ostream) const
{
    return BinaryConnected::serialise_binary_connected(
            ostream, operator_type, lhs.get(), rhs.get(), is_negative_polarity());
}

} // namespace optifol
