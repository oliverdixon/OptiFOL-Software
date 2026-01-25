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

bool operator<(const Resolvent &lhs, const Resolvent &rhs) noexcept
{
    const auto lhs_has_unit = lhs.lhs_clause.unit() == 1 || lhs.rhs_clause.unit() == 1;
    const auto rhs_has_unit = rhs.lhs_clause.unit() == 1 || rhs.rhs_clause.unit() == 1;

    if (lhs_has_unit && !rhs_has_unit)
        return false; // LHS has higher priority.

    if (!lhs_has_unit && rhs_has_unit)
        return true; // RHS has higher priority.

    return false; // Equal priority.
}

std::ostream &operator<<(std::ostream &ostream, const Resolvent &resolvent)
{
    return ostream << resolvent.resolution;
}

} // namespace optifol
