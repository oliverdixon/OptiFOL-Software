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
                 const TimeT &created_time,
                 const TimeT &last_modified_time):
    created_time(created_time),
    last_modified_time(last_modified_time),
    name(name),
    id(id)
{
}

std::string Project::get_identifier() const
{
    return name;
}

IStorageObject::TimeT Project::get_creation_time() const
{
    return created_time;
}

IStorageObject::TimeT Project::get_modified_time() const
{
    return last_modified_time;
}

std::size_t Project::get_controller_id() const noexcept
{
    return id;
}

bool Project::operator==(const Project &other) const noexcept
{
    return id == other.id;
}

bool Project::operator==(std::size_t other_id) const noexcept
{
    return id == other_id;
}

}
