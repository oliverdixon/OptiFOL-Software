/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the project-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#include "Project.hpp"

namespace optifol
{

Project::Project(const std::size_t id,
                 const std::string &name,
                 const time_t &created_time,
                 const time_t &last_modified_time):
    id(id),
    name(name),
    created_time(created_time),
    last_modified_time(last_modified_time)
{
}

std::string Project::get_identifier() const
{
    return name;
}

IStorageObject::time_t Project::get_creation_time() const
{
    return created_time;
}

IStorageObject::time_t Project::get_modified_time() const
{
    return last_modified_time;
}

std::size_t Project::get_controller_id() const
{
    return id;
}

}
