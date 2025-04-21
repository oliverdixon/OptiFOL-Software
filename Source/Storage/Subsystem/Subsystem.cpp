/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the subsystem-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#include "Subsystem.hpp"

#include "../../Logging.hpp"

namespace optifol
{

Subsystem::Subsystem(const std::size_t id,
                 const std::size_t relevant_project_tag,
                 std::string &&name,
                 const TimeT &created_time,
                 const TimeT &last_modified_time):
    created_time(created_time),
    last_modified_time(last_modified_time),
    name(std::move(name)),
    id(id),
    relevant_project_tag(relevant_project_tag)
{
}

std::string Subsystem::get_identifier() const
{
    return name;
}

IStorageObject::TimeT Subsystem::get_creation_time() const
{
    return created_time;
}

IStorageObject::TimeT Subsystem::get_modified_time() const
{
    return last_modified_time;
}

std::size_t Subsystem::get_controller_id() const noexcept
{
    return id;
}

bool Subsystem::operator==(const Subsystem &other) const noexcept
{
    return id == other.id;
}

bool Subsystem::operator==(std::size_t other_id) const noexcept
{
    return id == other_id;
}

void Subsystem::set_identifier(const std::string &name_candidate)
{
    this->name = name_candidate;
    last_modified_time = std::chrono::system_clock::now();
}

void Subsystem::set_creation_time(const TimeT &time_candidate)
{
    this->created_time = time_candidate;
    last_modified_time = std::chrono::system_clock::now();
    LOG4CXX_WARN(Logging::get_logger(), "Updating the creation time of subsystem " << get_identifier());
}

void Subsystem::set_modified_time(const TimeT &time_candidate)
{
    this->last_modified_time = time_candidate;
}

void Subsystem::set_project_tag(const std::size_t project_tag_candidate)
{
    this->relevant_project_tag = project_tag_candidate;
}

std::size_t Subsystem::get_relevant_project_tag() const noexcept
{
    return relevant_project_tag;
}

}
