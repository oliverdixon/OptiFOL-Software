/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "Literal.hpp"

#include <utility>

namespace optifol
{

Literal::Literal(std::shared_ptr<PredicationNode> predicate, const bool is_positive):
    predicate(std::move(predicate)),
    is_positive(is_positive)
{}

}
