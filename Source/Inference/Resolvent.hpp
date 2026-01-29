/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the FOL Resolution resolvent
 * @author Oliver Dixon
 * @date 2026-01-25
 * @version Development
 */

#ifndef OPTIFOL_RESOLVENT_HPP
#define OPTIFOL_RESOLVENT_HPP

#include "../IR/Sentences/SentenceRoot.hpp"
#include "ResolventQueue.hpp"
#include "Unifier.hpp"

namespace optifol
{

class Resolvent : public IHashable, public ISerialisable
{
public:
    Resolvent(const Clause * lhs_clause, const Clause * rhs_clause, Unifier unifier, const Clause * resolution);

    Resolvent(Resolvent&&) = default;
    Resolvent& operator=(Resolvent&&) = default;

    [[nodiscard]] bool operator<(const Resolvent& other) const noexcept;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    [[nodiscard]] bool operator==(const Resolvent &other) const;

    [[nodiscard]] const Clause *observe_lhs_clause() const noexcept;

    [[nodiscard]] const Clause *observe_rhs_clause() const noexcept;

    [[nodiscard]] const Clause *observe_resolution() const noexcept;

private:
    friend void ResolventQueue::push(Resolvent, std::unique_ptr<Clause> &&);

    const Clause * lhs_clause;
    const Clause * rhs_clause;

    Unifier unifier;
    const Clause * resolution;
};

} // namespace optifol

#endif // OPTIFOL_RESOLVENT_HPP
