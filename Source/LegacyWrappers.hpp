/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef LEGACYWRAPPERS_HPP
#define LEGACYWRAPPERS_HPP

#include <chrono>
#include <cstddef>

namespace optifol
{

class LegacyWrappers
{
public:
    static std::size_t get_pid();
};

}

#if __cpp_lib_chrono < 202306L

// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class std::hash<std::chrono::system_clock::time_point>
 * @brief Standard hasher specialisation for the system clock, only required prior to C++26.
 */
template<>
struct std::hash<std::chrono::system_clock::time_point>
{
    std::size_t operator()(const std::chrono::system_clock::time_point& time) const noexcept;
};

#endif

#endif
