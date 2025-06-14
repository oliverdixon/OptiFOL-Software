/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "Predicate.hpp"

#include "../../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

Predicate::Predicate(std::string name, const bool is_positive, const std::initializer_list<const IProcessedTerm *> arguments) :
    name(std::move(name)), is_positive(is_positive)
{
    this->arguments.reserve(arguments.size());
    for (const auto argument: arguments)
        this->arguments.push_back(argument);
}

Predicate::Predicate(std::string name, std::initializer_list<const IProcessedTerm *> arguments, const bool is_positive) :
    Predicate(std::move(name), is_positive, std::move(arguments))
{
}

std::string Predicate::get_name() const
{
    return name;
}

void Predicate::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool Predicate::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::ostream &Predicate::serialise(std::ostream &ostream) const
{
    return ostream; // TODO URGENT
}

bool Predicate::accept(UnificationVisitor &visitor, const Predicate &target) const
{
    return visitor.visit(*this, target);
}

std::size_t Predicate::hash() const noexcept
{
    return 0; // TODO URGENT
}

const std::vector<const IProcessedTerm *> &Predicate::observe_arguments() const noexcept
{
    return arguments;
}

} // namespace optifol
