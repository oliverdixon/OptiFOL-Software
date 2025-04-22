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

#include "StorageHashFunctor.hpp"
#include "../Logging.hpp"

namespace optifol
{

Subsystem::Subsystem(std::string &&name, const TimeT &created_time, const TimeT &last_modified_time,
        Gio::ListStore<Subsystem> * model):
    TreeNode(model),
    created_time(created_time),
    last_modified_time(last_modified_time),
    name(std::move(name))
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

bool Subsystem::operator==(const Subsystem &other) const noexcept
{
    return std::hash<Subsystem>{}(*this) == std::hash<Subsystem>{}(other);
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

}
