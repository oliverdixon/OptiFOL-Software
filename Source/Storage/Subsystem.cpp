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

namespace optifol
{

Subsystem::Subsystem(const std::size_t id,
                 const std::string &name,
                 const TimeT &created_time,
                 const TimeT &last_modified_time):
    created_time(created_time),
    last_modified_time(last_modified_time),
    name(name),
    id(id)
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

}
