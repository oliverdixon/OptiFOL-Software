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
#include "Unifier.hpp"

namespace optifol
{

class Resolvent : public IHashable, public ISerialisable
{
public:
    Resolvent(Clause lhs_clause, Clause rhs_clause, Unifier unifier, Clause resolution);

    friend bool operator<(const Resolvent& lhs, const Resolvent& rhs) noexcept;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    friend std::ostream& operator<<(std::ostream& ostream, const Resolvent& resolvent);

    [[nodiscard]] const Clause& observe_lhs_clause() const noexcept;

    [[nodiscard]] const Clause& observe_rhs_clause() const noexcept;

    [[nodiscard]] const Clause& observe_resolution() const noexcept;

private:
    Clause lhs_clause;
    Clause rhs_clause;
    Unifier unifier;
    Clause resolution;
};

} // namespace optifol

#endif // OPTIFOL_RESOLVENT_HPP
