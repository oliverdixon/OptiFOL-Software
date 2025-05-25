/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 5/25/25.
//

#ifndef LITERAL_HPP
#define LITERAL_HPP

#include <memory>

namespace optifol
{

class PredicationNode;

/**
 * @class Literal
 * @brief A Literal is an instantiation of a Predicate with either positive or negative polarity. Literals are held
 *  under disjunction to form Clause objects.
 */
class Literal
{
public:
    explicit Literal(std::shared_ptr<PredicationNode> predicate, bool is_positive = true);

    const bool is_positive;
    std::shared_ptr<PredicationNode> predicate;
};

}

#endif
