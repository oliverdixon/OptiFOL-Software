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

#include "MutableIdentity.hpp"

#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

MutableIdentity::MutableIdentity(
        std::unique_ptr<IMutableTerm> &&lhs, std::unique_ptr<IMutableTerm> &&rhs, const bool is_positive) :
    lhs(std::move(lhs)),
    rhs(std::move(rhs)),
    is_positive(is_positive)
{
}

std::unique_ptr<IMutableSentence> MutableIdentity::clone() const
{
    return std::make_unique<MutableIdentity>(lhs->clone(), rhs->clone(), is_positive);
}

void MutableIdentity::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool MutableIdentity::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::unique_ptr<IMutableTerm> MutableIdentity::take_lhs_operand() noexcept
{
    return std::move(lhs);
}

std::unique_ptr<IMutableTerm> MutableIdentity::take_rhs_operand() noexcept
{
    return std::move(rhs);
}

const IMutableTerm *MutableIdentity::observe_lhs_operand() const noexcept
{
    return lhs.get();
}

const IMutableTerm *MutableIdentity::observe_rhs_operand() const noexcept
{
    return rhs.get();
}

void MutableIdentity::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutableIdentity::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

void MutableIdentity::accept(RepositoryBuildingVisitor &visitor)
{
    visitor.visit(*this);
}

std::size_t MutableIdentity::hash() const noexcept
{
    return hash_polarity(hash_combine_commutative(lhs->hash(), rhs->hash()), is_negative_polarity());
}

std::ostream &MutableIdentity::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(';
    lhs->serialise(ostream);
    ostream << " == ";
    return rhs->serialise(ostream) << ')';
}

void MutableIdentity::put_lhs_operand(std::unique_ptr<IMutableTerm> &&new_lhs) noexcept
{
    lhs = std::move(new_lhs);
}

void MutableIdentity::put_rhs_operand(std::unique_ptr<IMutableTerm> &&new_rhs) noexcept
{
    rhs = std::move(new_rhs);
}

} // namespace optifol
