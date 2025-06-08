/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "LegacyWrappers.hpp"

#include <unistd.h>

namespace optifol
{

std::size_t LegacyWrappers::get_pid()
{
    return static_cast<std::size_t>(getpid());
}

}

std::size_t std::hash<std::chrono::time_point<std::chrono::system_clock>>::operator()(
        const std::chrono::system_clock::time_point &time) const noexcept
{
    return static_cast<std::size_t>(time.time_since_epoch().count());
}
