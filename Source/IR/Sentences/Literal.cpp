/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Literal IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#include "Literal.hpp"

#include <algorithm>
#include <ranges>

#include "../../CompositeSerialisationHelpers.hpp"
#include "../../Inference/FVI/FeatureComponentBuilder.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

Literal::Literal(
        std::string name, const std::initializer_list<const IProcessedTerm *> arguments, const bool is_positive) :
    name(std::move(name)),
    arguments(arguments),
    is_positive(is_positive)
{
}

Literal::Literal(std::string name, std::vector<const IProcessedTerm *> &&arguments, const bool is_positive) :
    name(std::move(name)),
    arguments(std::move(arguments)),
    is_positive(is_positive)
{
}

bool Literal::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::ostream &Literal::serialise(std::ostream &ostream) const
{
    return CompositeSerialisationHelpers::stream_serialise(
            ostream, name, arguments.cbegin(), arguments.cend(), is_negative_polarity());
}

std::size_t Literal::hash() const noexcept
{
    return composite_hash(name, arguments.cbegin(), arguments.cend(), is_negative_polarity());
}

bool Literal::operator==(const IProcessedSentence &other) const noexcept
{
    return is_negative_polarity() == other.is_negative_polarity() && unsigned_equality(other);
}

bool Literal::operator<(const Literal &other) const noexcept
{
    // Order lexicographically on the string_view.
    if (get_name() < other.get_name())
        return true;

    if (get_name() > other.get_name())
        return false;

    // Names are lexicographically equal. Order on sign; negatives are considered less.
    if (is_negative_polarity() && !other.is_negative_polarity())
        return true;

    if (!is_negative_polarity() && other.is_negative_polarity())
        return false;

    // Literals are superficially equal, so order based on arguments.
    return std::ranges::any_of(std::ranges::views::zip(arguments, other.arguments), [](const auto& pair)
    {
        return *std::get<0>(pair) < *std::get<1>(pair);
    });
}

std::string_view Literal::get_name() const noexcept
{
    return name;
}

const std::vector<const IProcessedTerm *> &Literal::observe_arguments() const noexcept
{
    return arguments;
}

bool Literal::accept(UnificationVisitor &unification_visitor, const IProcessedSentence &sentence) const
{
    return sentence.accept(unification_visitor, *this);
}

bool Literal::accept(UnificationVisitor &unification_visitor, const Literal &predicate) const
{
    return unification_visitor.visit(*this, predicate);
}

const Literal *Literal::accept(const UnificationApplicationVisitor &application_visitor) const
{
    return application_visitor.visit(*this);
}

void Literal::accept(FeatureComponentBuilder &feature_builder) const
{
    feature_builder.visit(this);
}

bool Literal::unsigned_equality(const IProcessedSentence &other) const noexcept
{
    const auto other_literal = dynamic_cast<const Literal *>(&other);
    if (other_literal == nullptr)
        // Other IProcessedSentence isn't a Literal.
        return false;

    if (name != other_literal->name)
        // Other literal has a different superficial name.
        return false;

    const auto argument_count = arguments.size();
    if (argument_count != other_literal->arguments.size())
        // Other literal has a different number of arguments.
        return false;

    for (std::size_t arg_idx = 0; arg_idx < argument_count; ++arg_idx)
        if (*arguments[arg_idx] != *other_literal->arguments[arg_idx])
            // Other pairwise argument is different according to its own comparator.
            return false;

    return true;
}

std::ostream &operator<<(std::ostream &ostream, const Literal &literal)
{
    return literal.serialise(ostream);
}

} // namespace optifol
