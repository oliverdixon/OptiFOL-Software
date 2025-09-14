/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the binary-Connected Sentence IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#include <utility>

#include "../../Visitors/MutableTargets/Observers/LaTeXSerialisationVisitor.hpp"
#include "BinaryConnected.hpp"

namespace optifol
{

BinaryConnected::BinaryConnected(const BinaryOperatorTypes operator_type, const IProcessedSentence *const lhs,
        const IProcessedSentence *const rhs, const bool is_positive) :
    operator_type(operator_type),
    lhs(lhs),
    rhs(rhs),
    is_positive(is_positive)
{
}

bool BinaryConnected::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::size_t BinaryConnected::hash() const noexcept
{
    return hash_binary_connected(operator_type, lhs, rhs, is_negative_polarity());
}

std::ostream &BinaryConnected::serialise(std::ostream &ostream) const
{
    return serialise_binary_connected(ostream, operator_type, lhs, rhs, is_negative_polarity());
}

bool BinaryConnected::operator==(const IProcessedSentence &other) const noexcept
{
    const auto other_connected = dynamic_cast<const BinaryConnected *>(&other);
    if (other_connected == nullptr)
        // Other IProcessedSentence isn't a BinaryConnected.
        return false;

    return *lhs == *other_connected->lhs && *rhs == *other_connected->rhs;
}

BinaryOperatorTypes BinaryConnected::get_operator_type() const noexcept
{
    return operator_type;
}

const IProcessedSentence *BinaryConnected::observe_lhs_operand() const noexcept
{
    return lhs;
}

const IProcessedSentence *BinaryConnected::observe_rhs_operand() const noexcept
{
    return rhs;
}

std::size_t BinaryConnected::hash_binary_connected(const BinaryOperatorTypes operator_type, const ISentence *const lhs,
        const ISentence *const rhs, const bool is_negative_polarity) noexcept
{
    return hash_polarity(hash_combine(hash_combine_commutative(lhs->hash(), rhs->hash()),
                                 std::hash<std::size_t>{}(std::to_underlying(operator_type))),
            is_negative_polarity);
}

std::ostream &BinaryConnected::serialise_binary_connected(std::ostream &ostream,
        const BinaryOperatorTypes operator_type, const ISentence *const lhs, const ISentence *const rhs,
        const bool is_negative_polarity)
{
    if (is_negative_polarity)
        ostream << '~';

    ostream << '(';
    lhs->serialise(ostream);
    ostream << get_operator_symbol(operator_type);
    return rhs->serialise(ostream) << ')';
}

const char *BinaryConnected::get_operator_symbol(const BinaryOperatorTypes type)
{
    switch (type) {
    case BinaryOperatorTypes::Conjunction:
        return " & ";
    case BinaryOperatorTypes::Disjunction:
        return " | ";
    case BinaryOperatorTypes::Implication:
        return " => ";
    case BinaryOperatorTypes::Biconditional:
        return " <=> ";
    }

    return " ??? ";
}

} // namespace optifol
