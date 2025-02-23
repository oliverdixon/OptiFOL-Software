/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the subsystem-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP
#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class Subsystem
 * @brief The Subsystem storage object forms the second level of the OptiFOL object hierarchy; it belongs to a project,
 *  and consists of many individual requirements.
 */
class Subsystem :
    public IStorageObject
{
private:
    const time_t created_time;

    time_t last_modified_time;
    std::string name;
};

}

#endif //SUBSYSTEM_HPP
