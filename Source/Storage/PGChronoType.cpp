/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include <pqxx/pqxx>

#include "PGChronoType.hpp"

namespace
{

enum BufferRequirements
{
    Year = 4,
    Month = 2,
    Day = 2,
    Hour = 2,
    Minute = 2,
    Second = 2,
    Millisecond = 6,
    Delimiter = 1,
    Null = 1
};

int parse_digit(const char ch)
{
    const int digit = ch - '0';
    if (digit < 0 || digit > 9)
        throw pqxx::conversion_error{"Invalid timestamp format: invalid digit"};

    return digit;
}

int parse_numbers(const std::string_view str, std::size_t& idx, const char delim = 0, const std::size_t read_length = 2)
{
    if (str.size() - idx < read_length)
        throw pqxx::conversion_error{"Invalid timestamp format: insufficient characters to parse"};

    const std::size_t lower_bound_idx = idx;
    unsigned int last_multiplier = 1;
    unsigned int result = 0;

    for (std::size_t i = lower_bound_idx + read_length - 1; i > lower_bound_idx; --i) {
        result += parse_digit(str[i]) * last_multiplier;
        last_multiplier *= 10;
        ++idx;
    }

    result += parse_digit(str[lower_bound_idx]) * last_multiplier;

    // ReSharper disable once CppDFAConstantConditions
    if (delim != 0) {
        if (str[++idx] != delim)
            throw pqxx::conversion_error{"Invalid timestamp format: unexpected delimiter"};
        ++idx;
    }

    return result;
}

char* pad_number(int number, const std::size_t leading_capacity, char* const begin, const char delim = 0)
{
    std::size_t remaining_capacity = leading_capacity;

    // ReSharper disable once CppDFAConstantConditions
    if (delim != 0) {
        begin[remaining_capacity - 1] = delim;
        --remaining_capacity;
    }

    for (; number != 0; number /= 10) {
        if (remaining_capacity == 0)
            // We have another digit, but no leading space in the buffer
            throw pqxx::conversion_overrun{"Insufficient capacity to serialise the date-string into PostgreSQL "
                                           "format"};

        const unsigned int digit = number % 10;
        begin[remaining_capacity - 1] = digit + '0';
        --remaining_capacity;
    }

    // Pad the leading space with ASCII zeroes
    for (; remaining_capacity != 0; --remaining_capacity)
        begin[remaining_capacity - 1] = '0';

    return &begin[leading_capacity - 1];
}

std::tm parse_pg_timestamp(const std::string_view str)
{
    std::tm result;
    std::size_t idx = 0;

    result.tm_gmtoff = 0;
    result.tm_isdst = -1;

    result.tm_year = parse_numbers(str, idx, '-', 4) - 1900;
    result.tm_mon = parse_numbers(str, idx, '-') - 1;
    result.tm_mday = parse_numbers(str, idx, ' ');
    result.tm_hour = parse_numbers(str, idx, ':');
    result.tm_min = parse_numbers(str, idx, ':');
    result.tm_sec = parse_numbers(str, idx, '.');

    return std::move(result);
}

}

namespace pqxx
{

template<>
const std::string type_name<std::chrono::system_clock::time_point>{"Chrono time point"};

bool nullness<std::chrono::time_point<std::chrono::system_clock>>::is_null(
    const std::chrono::system_clock::time_point &time_point)
{
    return time_point.time_since_epoch() == std::chrono::duration<std::decay_t<decltype(time_point)>::rep>::zero();
}

std::chrono::system_clock::time_point nullness<std::chrono::time_point<std::chrono::system_clock>>::null()
{
    return {};
}

zview string_traits<std::chrono::time_point<std::chrono::system_clock>>::to_buf(char *begin, const char * const end,
    const std::chrono::system_clock::time_point &value)
{
    const char * const written_end = into_buf(begin, end, value);
    return {begin, written_end - begin};
}

char * string_traits<std::chrono::time_point<std::chrono::system_clock>>::into_buf(char * const begin,
    const char * const end, const std::chrono::system_clock::time_point &value)
{
    if (end - begin < string_traits::size_buffer(value))
        throw pqxx::conversion_overrun{"Insufficient capacity to serialise the date-string into PostgreSQL format"};

    char* cursor = begin;
    const std::time_t epoch_time = std::chrono::system_clock::to_time_t(value);
    const std::tm* tm{std::localtime(&epoch_time)};

    cursor = pad_number(tm->tm_year + 1900, BufferRequirements::Year + BufferRequirements::Delimiter, cursor, '-');
    cursor = pad_number(tm->tm_mon + 1, BufferRequirements::Month + BufferRequirements::Delimiter, cursor + 1, '-');
    cursor = pad_number(tm->tm_mday, BufferRequirements::Day + BufferRequirements::Delimiter, cursor + 1, ' ');
    cursor = pad_number(tm->tm_hour, BufferRequirements::Hour + BufferRequirements::Delimiter, cursor + 1, ':');
    cursor = pad_number(tm->tm_min, BufferRequirements::Minute + BufferRequirements::Delimiter, cursor + 1, ':');
    cursor = pad_number(tm->tm_sec, BufferRequirements::Second + BufferRequirements::Delimiter, cursor + 1, '.');

    ++cursor;
    for (std::size_t millisecond_idx = 0; millisecond_idx < BufferRequirements::Millisecond; ++millisecond_idx)
        *cursor++ = '0';

    *cursor = 0; // pqxx::zview requires NULL-termination
    return ++cursor;
}

std::size_t string_traits<std::chrono::time_point<std::chrono::system_clock>>::size_buffer(
    const std::chrono::system_clock::time_point &value) noexcept
{
    std::ignore = value;

    return BufferRequirements::Year +
           BufferRequirements::Delimiter +
           BufferRequirements::Month +
           BufferRequirements::Delimiter +
           BufferRequirements::Day +
           BufferRequirements::Delimiter +
           BufferRequirements::Hour +
           BufferRequirements::Delimiter +
           BufferRequirements::Minute +
           BufferRequirements::Delimiter +
           BufferRequirements::Second +
           BufferRequirements::Delimiter +
           BufferRequirements::Millisecond +
           BufferRequirements::Null;
}

std::chrono::system_clock::time_point string_traits<std::chrono::time_point<std::chrono::system_clock>>::from_string(
    const std::string_view text)
{
    auto tm = parse_pg_timestamp(text);
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

}
