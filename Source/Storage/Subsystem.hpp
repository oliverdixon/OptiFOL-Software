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
 * @brief The Subsystem storage object forms the second level of the OptiFOL object hierarchy; it belongs to a Project,
 *  and consists of many individual requirements.
 */
class Subsystem :
    public IStorageObject
{
    Subsystem(std::size_t id, const std::string& name, const TimeT& created_time, const TimeT& last_modified_time);

private:
    /**
     * @brief Time of initial creation
     * @note This quantity should be immutable in the model determined by the IStorageController, assigned only upon its
     *  initial creation.
     */
    const TimeT created_time;

    /**
     * @brief Time of most recent mutation
     * @note This quantity should be updated upon the changing of Subsystem metadata, or the changing of any data held
     *  by the Subsystem, such as any of its constituent requirements.
     */
    TimeT last_modified_time;

    /**
     * @brief The human-readable name of the Project
     */
    std::string name;

    /**
     * @brief The numerical ID of the Subsystem, unique up to being the IStorageController primary key for the Subsystem
     *  entity
     */
    const std::size_t id;
};

}

#endif //SUBSYSTEM_HPP
