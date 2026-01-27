/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the FOL Resolution resolvent
 * @author Oliver Dixon
 * @date 2026-01-25
 * @version Development
 */

#include "Resolvent.hpp"

#include <utility>

namespace optifol
{

Resolvent::Resolvent(Clause lhs_clause, Clause rhs_clause, Unifier unifier, Clause resolution) :
    lhs_clause(std::move(lhs_clause)),
    rhs_clause(std::move(rhs_clause)),
    unifier(std::move(unifier)),
    resolution(std::move(resolution))
{
}

std::size_t Resolvent::hash() const noexcept
{
    return resolution.hash();
}

std::ostream &Resolvent::serialise(std::ostream &ostream) const
{
    return ostream << resolution;
}

bool Resolvent::operator==(const Resolvent &other) const
{
    return resolution == other.resolution;
}

const Clause &Resolvent::observe_lhs_clause() const noexcept
{
    return lhs_clause;
}

const Clause &Resolvent::observe_rhs_clause() const noexcept
{
    return rhs_clause;
}

const Clause &Resolvent::observe_resolution() const noexcept
{
    return resolution;
}

bool Resolvent::operator<(const Resolvent &other) const noexcept
{
    const auto lhs_has_unit = lhs_clause.unit() || rhs_clause.unit();
    const auto rhs_has_unit = other.lhs_clause.unit() || other.rhs_clause.unit();

    if (lhs_has_unit && !rhs_has_unit)
        return false; // LHS has higher priority.

    if (!lhs_has_unit && rhs_has_unit)
        return true; // RHS has higher priority.

    // Equal priority. For resolvents, we don't care about the exact ordering provided by Clause::operator<.
    return false;
}

} // namespace optifol
