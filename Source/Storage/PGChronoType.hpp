/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PGCHRONOTYPE_HPP
#define PGCHRONOTYPE_HPP

#include <chrono>
#include <pqxx/strconv>
#include <pqxx/zview>

namespace pqxx
{

template<>
struct nullness<std::chrono::system_clock::time_point>
{
    static constexpr bool has_null{false};

    static constexpr bool always_null{false};

    static bool is_null(const std::chrono::system_clock::time_point& time_point);

    [[nodiscard]] static std::chrono::system_clock::time_point null();
};

template<>
struct string_traits<std::chrono::system_clock::time_point>
{
    static constexpr bool converts_to_string{true};

    static constexpr bool converts_from_string{true};

    static zview to_buf(char *begin, const char * end, const std::chrono::system_clock::time_point& value);

    static char * into_buf(char *begin, const char *end, const std::chrono::system_clock::time_point& value);

    static std::size_t size_buffer(const std::chrono::system_clock::time_point& value) noexcept;

    static std::chrono::system_clock::time_point from_string(std::string_view text);
};

}

#endif
