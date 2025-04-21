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

#include "../../Logging.hpp"

namespace optifol
{

Project::Project(const std::size_t id,
                 std::string &&name,
                 const TimeT &created_time,
                 const TimeT &last_modified_time):
    created_time(created_time),
    last_modified_time(last_modified_time),
    name(std::move(name)),
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

void Project::set_identifier(const std::string &name_candidate)
{
    this->name = name_candidate;
    last_modified_time = std::chrono::system_clock::now();
}

void Project::set_creation_time(const TimeT &time_candidate)
{
    this->created_time = time_candidate;
    last_modified_time = std::chrono::system_clock::now();
    LOG4CXX_WARN(Logging::get_logger(), "Updating the creation time of project " << get_identifier());
}

void Project::set_modified_time(const TimeT &time_candidate)
{
    this->last_modified_time = time_candidate;
}

}
