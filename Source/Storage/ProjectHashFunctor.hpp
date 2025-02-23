/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PROJECTHASHFUNCTOR_HPP
#define PROJECTHASHFUNCTOR_HPP

#include "Project.hpp"

template<>
struct std::hash<optifol::Project>
{
    std::size_t operator()(const optifol::Project& project) const noexcept;
};

namespace optifol
{

struct ProjectHashFunctor
{
    using hash_type = std::hash<Project>;
    using is_transparent = void;

    std::size_t operator()(const Project& project) const noexcept;

    std::size_t operator()(std::size_t project_id) const noexcept;
};

}

#endif
