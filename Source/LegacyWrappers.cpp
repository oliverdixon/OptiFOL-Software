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
