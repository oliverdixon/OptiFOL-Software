/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP

#include "Requirement.hpp"

namespace optifol
{

class Subsystem
{
public:
    explicit Subsystem(std::string&& name);

private:
    const std::string name;
};

}

#endif
