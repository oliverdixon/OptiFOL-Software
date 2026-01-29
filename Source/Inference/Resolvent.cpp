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

Resolvent::Resolvent(const ProofTreeNode * const lhs_parent, const ProofTreeNode * const rhs_parent, Unifier unifier,
        const Clause * resolution) :
    ProofTreeNode(lhs_parent, rhs_parent),
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

bool Resolvent::is_unit() const noexcept
{
    return resolution->is_unit();
}

const Clause *Resolvent::observe_substance() const noexcept
{
    return resolution;
}

bool Resolvent::operator<(const Resolvent &other) const noexcept
{
    const auto we_have_unit = observe_lhs_parent()->is_unit() || observe_rhs_parent()->is_unit();
    const auto other_has_unit = other.observe_lhs_parent()->is_unit() || other.observe_rhs_parent()->is_unit();

    if (we_have_unit && !other_has_unit)
        return false; // LHS has higher priority.

    if (!we_have_unit && other_has_unit)
        return true; // RHS has higher priority.

    // Equal priority. For resolvents, we don't care about the exact ordering provided by Clause::operator<.
    return false;
}

} // namespace optifol
