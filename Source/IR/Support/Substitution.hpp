/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Variable-Term Substitution Map
 * @author Oliver Dixon
 * @date 2025-06-07
 * @version Development
 */

#ifndef SUBSTITUTION_HPP
#define SUBSTITUTION_HPP
#include <unordered_map>

#include "../Terms/VariableNode.hpp"

namespace optifol
{

class Substitution
{
public:
    // TODO
    std::unordered_map<std::shared_ptr<VariableNode>, std::shared_ptr<ITermNode>> bindings;
};

}

#endif //SUBSTITUTION_HPP
