/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Identity Sentence IR node
 * @author Oliver Dixon
 * @date 2025-06-18
 * @version Development
 */

#include "Identity.hpp"

#include "../Terms/IProcessedTerm.hpp"

namespace optifol
{

Identity::Identity(const IProcessedTerm * const lhs, const IProcessedTerm * const rhs, const bool is_positive) :
    lhs(lhs),
    rhs(rhs),
    is_positive(is_positive)
{
}

bool Identity::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::ostream &Identity::serialise(std::ostream &ostream) const
{
    return Identity::serialise_identity(ostream, lhs, rhs, is_negative_polarity());
}

std::size_t Identity::hash() const noexcept
{
    return Identity::hash_identity(lhs, rhs, is_negative_polarity());
}

bool Identity::operator==(const IProcessedSentence &other) const noexcept
{
    const auto other_identity = dynamic_cast<const Identity *>(&other);
    if (other_identity == nullptr)
        // Other IProcessedSentence isn't an Identity.
        return false;

    return commutative_ptr_compare(lhs, rhs, other_identity->lhs, other_identity->rhs);
}

const IProcessedTerm *Identity::observe_lhs_operand() const noexcept
{
    return lhs;
}

const IProcessedTerm *Identity::observe_rhs_operand() const noexcept
{
    return rhs;
}

std::size_t Identity::hash_identity(const ITerm * const lhs, const ITerm * const rhs,
    const bool is_negative_polarity) noexcept
{
    return hash_polarity(hash_combine_commutative(lhs->hash(), rhs->hash()), is_negative_polarity);
}

std::ostream &Identity::serialise_identity(
        std::ostream &ostream, const ITerm * const lhs, const ITerm * const rhs, const bool is_negative_polarity)
{
    if (is_negative_polarity)
        ostream << '~';

    ostream << '(';
    lhs->serialise(ostream);
    ostream << " == ";
    return rhs->serialise(ostream) << ')';
}

} // namespace optifol
