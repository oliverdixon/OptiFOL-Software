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

#include "../../CompositeSerialisationHelpers.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

Literal::Literal(
        std::string name, std::initializer_list<const IProcessedTerm *> arguments, const bool is_positive) :
    name(std::move(name)),
    arguments(arguments),
    is_positive(is_positive)
{
}

Literal::Literal(std::string name, std::vector<const IProcessedTerm *> &&arguments, bool is_positive) :
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

} // namespace optifol
