/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the requirement-level storage object
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#include "Requirement.hpp"

namespace optifol
{

Requirement::Requirement(const std::size_t id, const std::string &name, const TimeT &created_time,
        const TimeT &last_modified_time, const std::string& statement, const std::size_t priority) :
    created_time(created_time),
    last_modified_time(last_modified_time),
    name(name),
    id(id),
    statement(statement),
    priority(priority)
{
}

std::string Requirement::get_identifier() const
{
    return name;
}

IStorageObject::TimeT Requirement::get_creation_time() const
{
    return created_time;
}

IStorageObject::TimeT Requirement::get_modified_time() const
{
    return last_modified_time;
}

std::size_t Requirement::get_controller_id() const noexcept
{
    return id;
}

bool Requirement::operator==(const Requirement &other) const noexcept
{
    return id == other.id;
}

bool Requirement::operator==(const std::size_t other_id) const noexcept
{
    return id == other_id;
}

std::string Requirement::get_statement() const
{
    return statement;
}

std::size_t Requirement::get_priority() const
{
    return priority;
}

}
