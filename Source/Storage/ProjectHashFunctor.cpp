/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "ProjectHashFunctor.hpp"

std::size_t std::hash<optifol::Project>::operator()(const optifol::Project &project) const noexcept
{
    return project.get_controller_id();
}

namespace optifol
{

std::size_t ProjectHashFunctor::operator()(const Project &project) const noexcept
{
    return project.get_controller_id();
}

std::size_t ProjectHashFunctor::operator()(const std::size_t project_id) const noexcept
{
    return project_id;
}

}
