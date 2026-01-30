/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#include "OccurrenceCountFeature.hpp"

#include "../../IR/Sentences/Literal.hpp"
#include "../../IR/Terms/IProcessedTerm.hpp"

namespace optifol
{

OccurrenceCountFeature::OccurrenceCountFeature(const bool polarity) :
    polarity(polarity)
{
}

OccurrenceCountFeature::OccurrenceCountFeature(const IProcessedTerm * const target, const bool polarity) :
    target(target),
    polarity(polarity)
{
}

OccurrenceCountFeature::OccurrenceCountFeature(const Literal * const target, const bool polarity) :
    target(target),
    polarity(polarity)
{
}

std::size_t OccurrenceCountFeature::hash() const noexcept
{
    if (!target.has_value())
        return polarity;

    return std::visit([](auto&& arg) { return static_cast<const IHashable *>(arg)->hash(); }, *target);
}

bool OccurrenceCountFeature::operator==(const OccurrenceCountFeature &other_feature) const noexcept
{
    const bool have_values = target.has_value();
    if (have_values != other_feature.target.has_value())
        return false;

    if (!have_values)
        return polarity == other_feature.polarity;

    return std::visit(EqualityComparator{}, *target, *other_feature.target);
}

bool OccurrenceCountFeature::operator==(bool other_polarity) const noexcept
{
    if (target.has_value())
        return true;

    return polarity == other_polarity;
}

bool OccurrenceCountFeature::operator==(const Literal *literal_candidate) const noexcept
{
    if (!target.has_value())
        return false;

    const auto literal_target = std::get_if<const Literal *>(&*target);
    if (literal_target == nullptr)
        return false;

    return (*literal_target)->operator==(*literal_candidate);
}

bool OccurrenceCountFeature::operator==(const IProcessedTerm *term_candidate) const noexcept
{
    if (!target.has_value())
        return false;

    const auto term_target = std::get_if<const IProcessedTerm *>(&*target);
    if (term_target == nullptr)
        return false;

    return (*term_target)->operator==(*term_candidate);
}

bool OccurrenceCountFeature::operator<(const OccurrenceCountFeature &other_feature) const noexcept
{
    if (target.has_value()) {
        if (other_feature.target.has_value())
            return std::visit(LessThanComparator{}, *target, *other_feature.target);

        return false;
    }

    if (other_feature.target.has_value())
        return true;

    return std::less{}(polarity, other_feature.polarity);
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
    const bool polarity_lhs, const bool polarity_rhs) const noexcept
{
    return polarity_lhs == polarity_rhs;
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const bool polarity_lhs, const IProcessedTerm *term_rhs) const noexcept
{
    std::ignore = polarity_lhs;
    std::ignore = term_rhs;
    return false;
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const bool polarity_lhs, const Literal *literal_rhs) const noexcept
{
    std::ignore = polarity_lhs;
    std::ignore = literal_rhs;
    return false;
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const IProcessedTerm *term_lhs, const IProcessedTerm *term_rhs) const noexcept
{
    return term_lhs->operator==(*term_rhs);
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const IProcessedTerm *term_lhs, const Literal *literal_rhs) const noexcept
{
    std::ignore = term_lhs;
    std::ignore = literal_rhs;
    return false;
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const IProcessedTerm *term_lhs, const bool polarity_rhs) const noexcept
{
    std::ignore = term_lhs;
    std::ignore = polarity_rhs;
    return false;
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const Literal *literal_lhs, const IProcessedTerm *term_rhs) const noexcept
{
    std::ignore = literal_lhs;
    std::ignore = term_rhs;
    return false;
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const Literal *literal_lhs, const Literal *literal_rhs) const noexcept
{
    return literal_lhs->operator==(*literal_rhs);
}

bool OccurrenceCountFeature::EqualityComparator::operator()(
        const Literal *literal_lhs, const bool polarity_rhs) const noexcept
{
    std::ignore = literal_lhs;
    std::ignore = polarity_rhs;
    return false;
}

bool OccurrenceCountFeature::LessThanComparator::operator()(
        const IProcessedTerm *term_lhs, const IProcessedTerm *term_rhs) const noexcept
{
    return std::less{}(*term_lhs, *term_rhs);
}

bool OccurrenceCountFeature::LessThanComparator::operator()(
        const IProcessedTerm *term_lhs, const Literal *literal_rhs) const noexcept
{
    std::ignore = term_lhs;
    std::ignore = literal_rhs;
    return true;
}

bool OccurrenceCountFeature::LessThanComparator::operator()(
        const Literal *literal_lhs, const IProcessedTerm *term_rhs) const noexcept
{
    std::ignore = literal_lhs;
    std::ignore = term_rhs;
    return false;
}

bool OccurrenceCountFeature::LessThanComparator::operator()(
        const Literal *literal_lhs, const Literal *literal_rhs) const noexcept
{
    return std::less{}(*literal_rhs, *literal_lhs);
}

} // namespace optifol
