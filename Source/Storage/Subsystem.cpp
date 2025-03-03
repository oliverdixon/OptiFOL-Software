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

}
