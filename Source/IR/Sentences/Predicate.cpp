/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Predicate IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#include "Predicate.hpp"

#include "../../CompositeSerialisationHelpers.hpp"
#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

Predicate::Predicate(
        std::string name, const bool is_positive, const std::initializer_list<const IProcessedTerm *> arguments) :
    name(std::move(name)),
    arguments(arguments),
    is_positive(is_positive)
{
}

Predicate::Predicate(
        std::string name, std::initializer_list<const IProcessedTerm *> arguments, const bool is_positive) :
    Predicate(std::move(name), is_positive, arguments)
{
}

bool Predicate::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::ostream &Predicate::serialise(std::ostream &ostream) const
{
    return CompositeSerialisationHelpers::stream_serialise(
            ostream, name, arguments.cbegin(), arguments.cend(), is_negative_polarity());
}

bool Predicate::accept(UnificationVisitor &visitor, const Predicate &target) const
{
    return visitor.visit(*this, target);
}

bool Predicate::operator==(const std::unique_ptr<Predicate> &other) const noexcept
{
    return other->hash() == hash();
}

bool Predicate::operator==(const std::shared_ptr<Predicate> &other) const noexcept
{
    return other->hash() == hash();
}

std::size_t Predicate::hash() const noexcept
{
    return composite_hash(name, arguments.cbegin(), arguments.cend(), is_negative_polarity());
}

std::string_view Predicate::get_name() const noexcept
{
    return name;
}

const std::vector<const IProcessedTerm *> &Predicate::observe_arguments() const noexcept
{
    return arguments;
}

} // namespace optifol
