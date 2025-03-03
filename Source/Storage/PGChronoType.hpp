/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PQXX chronographic serialiser and deserialiser specialisations
 * @author Oliver Dixon
 * @date 2025-02-28
 * @version Development
 */

#ifndef PGCHRONOTYPE_HPP
#define PGCHRONOTYPE_HPP

#include <chrono>
#include <pqxx/strconv>
#include <pqxx/zview>

namespace pqxx
{

// Disabling this warning, as ReSharper cannot presently resolve name subject to template specialisation
// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class nullness<std::chrono::system_clock::time_point>
 * @brief Nullness sentinel, specialised the standard chronographic type
 * @see https://github.com/jtv/libpqxx/blob/master/include/pqxx/doc/datatypes.md#specialise-nullness
 */
template<>
struct nullness<std::chrono::system_clock::time_point>
{
    /**
     * @brief Does there exist an instance of the chronographic type expressive of a null value?
     */
    static constexpr bool has_null{false};

    /**
     * @brief Is any instance of the chronographic type expressive of its null equivalent?
     */
    static constexpr bool always_null{false};

    /**
     * @brief Comparator to the null chronographic type
     * @param time_point The value to compare
     * @return Is the given value representative of a null value?
     */
    static bool is_null(const std::chrono::system_clock::time_point& time_point);

    /**
     * @brief Constructs a canonical null chronographic type
     * @return The null chronographic type
     */
    [[nodiscard]] static std::chrono::system_clock::time_point null();
};

// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class string_traits<std::chrono::system_clock::time_point>
 * @brief String traits definition, specialised for the standard chronographic type
 * @see https://github.com/jtv/libpqxx/blob/master/include/pqxx/doc/datatypes.md#specialise-string_traits
 */
template<>
struct string_traits<std::chrono::system_clock::time_point>
{
    /**
     * @brief Can the chronographic type be serialised into a string?
     */
    static constexpr bool converts_to_string{true};

    /**
     * @brief Convert the given chronographic value into a PostgreSQL-friendly string
     * @param begin The beginning of the destination buffer
     * @param end The past-end of the destination buffer
     * @param value The chronographic value to serialise
     * @return A view to the serialised null-terminated string
     * @throws pqxx::conversion_overrun The value could not be serialised into the given buffer
     * @see https://github.com/jtv/libpqxx/blob/master/include/pqxx/doc/datatypes.md#to_buf
     */
    static zview to_buf(char *begin, const char * end, const std::chrono::system_clock::time_point& value);

    /**
     * @brief Convert the given chronographic value into a PostgreSQL-friendly string
     * @param begin The beginning of the destination buffer
     * @param end The past-end of the destination buffer
     * @param value The chronographic value to serialise
     * @return A view to the serialised null-terminated string
     * @throws pqxx::conversion_overrun The value could not be serialised into the given buffer
     * @note Although PQXX proposes that this function is a stricter version of the other serialiser, this
     *  specialisation provides the same guarantees RE buffer starting position and null-termination.
     * @see https://github.com/jtv/libpqxx/blob/master/include/pqxx/doc/datatypes.md#into_buf
     */
    static char * into_buf(char *begin, const char *end, const std::chrono::system_clock::time_point& value);

    /**
     * @brief Determines the minimum buffer capacity required to serialise the given chronographic value
     * @param value The chronographic value to be serialised
     * @return The minimum number of bytes consumed by a null-terminated serialised variant of the given value
     * @see https://github.com/jtv/libpqxx/blob/master/include/pqxx/doc/datatypes.md#size_buffer
     */
    static std::size_t size_buffer(const std::chrono::system_clock::time_point& value) noexcept;

    /**
     * @brief Can the chronographic type be parsed from a string?
     */
    static constexpr bool converts_from_string{true};

    /**
     * @brief Parse the given string into the standard chronographic type
     * @details The given string is expected to be in the format YYYY-mm-DD HH:MM:SS. We also assume an ASCII-like
     *  encoding as a precondition for the digit conversion.
     * @param text The string received from the JSON document
     * @return The parsed type
     * @throws pqxx::conversion_error The given string is not a valid date-string
     * @see https://github.com/jtv/libpqxx/blob/master/include/pqxx/doc/datatypes.md#from_string
     */
    static std::chrono::system_clock::time_point from_string(std::string_view text);
};

}

#endif
