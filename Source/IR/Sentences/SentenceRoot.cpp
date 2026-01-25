/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-06-23
 * @version Development
 */

#include "SentenceRoot.hpp"

#include <algorithm>

#include "Literal.hpp"

namespace optifol
{

bool SentenceRoot::is_negative_polarity() const noexcept
{
    return false;
}

std::ostream &SentenceRoot::serialise(std::ostream &ostream) const
{
    ostream << '{' << ' ';

    if (!clauses.empty()) {
        std::ranges::for_each_n(clauses.begin(), clauses.size() - 1, [&ostream](const Clause& clause)
        {
            ostream << clause << ',' << ' ';
        });

        ostream << clauses.back();
    }

    return ostream << ' ' << '}';
}

std::size_t SentenceRoot::hash() const noexcept
{
    std::size_t hash_value = clauses.size();

    for (const auto &clause: clauses) {
        std::size_t literal_hash_value = clause.order();
        for (const auto literal: clause)
            literal_hash_value = hash_combine(literal_hash_value, literal->hash());

        hash_value = hash_combine_commutative(hash_value, literal_hash_value);
    }

    return hash_value;
}

bool SentenceRoot::operator==(const IProcessedSentence &other) const noexcept
{
    const auto other_root = dynamic_cast<const SentenceRoot *>(&other);
    if (other_root == nullptr)
        // Other IProcessedSentence isn't a SentenceRoot.
        return false;

    // We have an ordering on the Clauses, so this is OK.
    return std::ranges::equal(clauses, other_root->clauses);
}

void SentenceRoot::add_clause(const Clause& new_clause)
{
    /*
     * Don't use std::ranges::lower_bound here. Clause doesn't model std::totally_ordered_with because it defines only
     * operator<, hence std::ranges::less cannot be used, and it's not worth the hassle of defining a custom comparator
     * here.
     */
    const auto nearest_lower = std::lower_bound(clauses.begin(), clauses.end(), new_clause);

    if ((nearest_lower != clauses.end() && *nearest_lower == new_clause) ||
            (nearest_lower != clauses.begin() && *std::prev(nearest_lower) == new_clause))
        /*
         * Reject the new clause if it already exists within the SentenceRoot. Note that the only two positions at which
         * it could appear, assuming correct lexicographical ordering provided by Clause::operator<, is at the proposed
         * insertion point or immediately prior.
         */
        return;

    clauses.insert(nearest_lower, new_clause);
}

std::vector<Clause>::const_iterator SentenceRoot::begin() const noexcept
{
    return clauses.begin();
}

std::vector<Clause>::const_iterator SentenceRoot::end() const noexcept
{
    return clauses.end();
}

std::size_t SentenceRoot::order() const noexcept
{
    return clauses.size();
}

} // namespace optifol
