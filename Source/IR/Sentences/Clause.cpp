/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 9/15/25.
//

#include "Clause.hpp"

namespace optifol
{

Clause::Clause(const std::initializer_list<const Literal *> literals) :
    literals(literals)
{
}

void Clause::add_literal(const Literal *literal)
{
    literals.push_back(literal);
}

decltype(Clause::literals)::const_iterator Clause::begin() const noexcept
{
    return literals.cbegin();
}

decltype(Clause::literals)::const_iterator Clause::end() const noexcept
{
    return literals.cend();
}

std::ostream &operator<<(std::ostream &ostream, const Clause &clause)
{

}

} // namespace optifol
