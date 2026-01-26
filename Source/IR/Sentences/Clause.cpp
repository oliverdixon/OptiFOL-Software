/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Clause collection
 * @author Oliver Dixon
 * @date 2025-09-15
 * @version Development
 */

#include "Clause.hpp"

#include <algorithm>
#include <ranges>

namespace optifol
{

Clause::Clause(const std::initializer_list<const Literal *> literals) :
    literals(literals)
{
    if (!this->literals.empty()) {
        std::ranges::sort(this->literals, [](const Literal * const lhs, const Literal * const rhs)
        {
            return *lhs < *rhs;
        });

        if (is_tautology()) {
            this->literals.clear();
            state = State::TriviallyTrue;
        } else
            state = State::NotTrivial;
    }
}

void Clause::add_literal(const Literal * const new_literal)
{
    const auto nearest_lower = std::ranges::lower_bound(
        literals, new_literal, [](const Literal * const lhs, const Literal * const rhs)
        {
            return *lhs < *rhs;
        }
    );

    /*
     * The nearest lower element is already either identical to the new literal, or its complement. In the former case,
     * we can omit adding it entirely; in the latter case, the entire clause becomes trivial.
     */
    if (nearest_lower != literals.cend() && (*nearest_lower)->unsigned_equality(*new_literal)) {
        if ((*nearest_lower)->is_negative_polarity() != new_literal->is_negative_polarity()) {
            literals.clear();
            state = State::TriviallyTrue;
        }

        return;
    }

    // Do a similar check just prior to the insertion point, if this could identify a tautology.
    if (nearest_lower != literals.cbegin()) {
        const auto previous_literal = std::prev(nearest_lower);
        if ((*previous_literal)->unsigned_equality(*new_literal)) {
            if ((*previous_literal)->is_negative_polarity() != new_literal->is_negative_polarity()) {
                literals.clear();
                state = State::TriviallyTrue;
            }

            return;
        }
    }

    // If this is a new unseen literal, insert at the correct position to maintain ordering i.a.w. Literal::operator<.
    literals.insert(nearest_lower, new_literal);
    state = State::NotTrivial;
}

Clause::State Clause::get_triviality_state() const noexcept
{
    return state;
}

decltype(Clause::literals)::const_iterator Clause::begin() const noexcept
{
    return literals.cbegin();
}

decltype(Clause::literals)::const_iterator Clause::end() const noexcept
{
    return literals.cend();
}

std::ostream &Clause::serialise(std::ostream &ostream) const
{
    ostream << '{' << ' ';

    if (!literals.empty()) {
        std::ranges::for_each_n(
            literals.begin(),
            literals.size() - 1, // NOLINT(*-narrowing-conversions)
            [&ostream](const Literal * literal)
            {
                ostream << *literal << ',' << ' ';
            }
        );

        ostream << *literals.back();
    }

    return ostream << ' ' << '}';
}

std::size_t Clause::hash() const noexcept
{
    return std::ranges::fold_left(
        literals, std::size_t{0},
        [](const std::size_t seed, const Literal * const literal)
        {
            return hash_combine(seed, std::hash<Literal>{}(*literal));
        });
}

bool Clause::operator<(const Clause &other) const noexcept
{
    return std::ranges::lexicographical_compare(literals, other.literals, [](const Literal * lhs, const Literal * rhs)
    {
        return *lhs < *rhs;
    });
}

bool Clause::operator==(const Clause &other) const noexcept
{
    return std::ranges::equal(
        literals,
        other.literals,
        [](const Literal * lhs, const Literal * rhs)
        {
            return lhs->operator==(*rhs);
        }
    );
}

std::size_t Clause::order() const noexcept
{
    return literals.size();
}

bool Clause::empty() const noexcept
{
    return literals.empty();
}

bool Clause::unit() const noexcept
{
    return order() == 1;
}

void Clause::force_bottom() noexcept
{
    literals.clear();
    state = State::TriviallyFalse;
}

bool Clause::is_tautology() const noexcept
{
    return std::ranges::adjacent_find(literals,
        [](const Literal * lhs, const Literal * rhs)
        {
            return lhs->unsigned_equality(*rhs) && lhs->is_negative_polarity() == !rhs->is_negative_polarity();
        }) != literals.end();
}

} // namespace optifol
