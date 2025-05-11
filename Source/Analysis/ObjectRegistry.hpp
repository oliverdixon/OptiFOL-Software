/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the SAT Engine object registry
 * @author Oliver Dixon
 * @date 2025-05-11
 * @version Development
 */

#ifndef OBJECTREGISTRY_HPP
#define OBJECTREGISTRY_HPP

#include <vector>

#include "Clause.hpp"
#include "Literal.hpp"
#include "Variable.hpp"

namespace optifol
{

class ObjectRegistry
{
private:
    std::vector<Clause> clauses;

    std::vector<Literal> literals;

    std::vector<Variable> variables;
};

}

#endif
