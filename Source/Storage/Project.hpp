/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the project-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class Project
 * @brief The Project storage forms the top level of the OptiFOL object hierarchy; it contains many subsystems.
 */
class Project :
        public IStorageObject
{
private:
    const time_t created_time;

    time_t last_modified_time;
    std::string name;
};

}

#endif
