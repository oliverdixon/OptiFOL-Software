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

#include "../../Sentences/Identity.hpp"

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

const ISentence *MutableIdentity::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

std::size_t MutableIdentity::hash() const noexcept
{
    return Identity::hash_identity(lhs.get(), rhs.get(), is_negative_polarity());
}

std::ostream &MutableIdentity::serialise(std::ostream &ostream) const
{
    return Identity::serialise_identity(ostream, lhs.get(), rhs.get(), is_negative_polarity());
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
