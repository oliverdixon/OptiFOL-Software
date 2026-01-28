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

#include <algorithm>
#include <ranges>
#include <utility>

namespace optifol
{

Resolvent::Resolvent(const Clause * const lhs_clause, const Clause * const rhs_clause, Unifier unifier,
        const Clause * resolution) :
    lhs_clause(lhs_clause),
    rhs_clause(rhs_clause),
    unifier(std::move(unifier)),
    resolution(resolution)
{
}

std::size_t Resolvent::hash() const noexcept
{
    return resolution->hash();
}

std::ostream &Resolvent::serialise(std::ostream &ostream) const
{
    return ostream << *resolution;
}

bool Resolvent::operator==(const Resolvent &other) const
{
    return resolution == other.resolution;
}

const Clause *Resolvent::observe_lhs_clause() const noexcept
{
    return lhs_clause;
}

const Clause *Resolvent::observe_rhs_clause() const noexcept
{
    return rhs_clause;
}

const Clause *Resolvent::observe_resolution() const noexcept
{
    return resolution;
}

void Resolvent::change_resolution(const Clause *clause) noexcept
{
    resolution = clause;
}

bool Resolvent::operator<(const Resolvent &other) const noexcept
{
    const auto lhs_has_unit = lhs_clause->is_unit() || rhs_clause->is_unit();
    const auto rhs_has_unit = other.lhs_clause->is_unit() || other.rhs_clause->is_unit();

    if (lhs_has_unit && !rhs_has_unit)
        return false; // LHS has higher priority.

    if (!lhs_has_unit && rhs_has_unit)
        return true; // RHS has higher priority.

    // Equal priority. For resolvents, we don't care about the exact ordering provided by Clause::operator<.
    return false;
}

} // namespace optifol
