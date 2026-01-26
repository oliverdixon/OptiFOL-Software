/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Unifier substitution map
 * @author Oliver Dixon
 * @date 2026-01-26
 * @version Development
 */

#include "Unifier.hpp"

#include <ranges>

namespace optifol
{

Unifier::Unifier() = default;

Unifier::Unifier(RawUnorderedMap<const Variable, const IProcessedTerm *> unifier) :
    unifier(std::move(unifier))
{ }

bool Unifier::operator==(const Unifier &other) const
{
    return unifier == other.unifier;
}

std::ostream &Unifier::serialise(std::ostream &ostream) const
{
    ostream << "{ ";

    const auto unifier_count_bound = unifier.size() - 1;

    for (const auto&& [idx, mapping] : std::ranges::enumerate_view(unifier)) {
        const auto [var, sub] = mapping;
        ostream << *var << '/' << *sub;
        if (static_cast<decltype(unifier_count_bound)>(idx) < unifier_count_bound)
            ostream << ", ";
    }

    return ostream << " }";
}

} // namespace optifol
