/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 9/15/25.
//

#ifndef OPTIFOL_CLAUSE_HPP
#define OPTIFOL_CLAUSE_HPP

#include <vector>

#include "Literal.hpp"

namespace optifol
{

class Literal;

class Clause
{
    std::vector<const Literal *> literals;

public:
    explicit Clause(std::initializer_list<const Literal *> literals);

    void add_literal(const Literal * literal);

    decltype(literals)::const_iterator begin() const noexcept;

    decltype(literals)::const_iterator end() const noexcept;

    friend std::ostream& operator<<(std::ostream& ostream, const Clause& clause);
};

} // namespace optifol

#endif // OPTIFOL_CLAUSE_HPP
