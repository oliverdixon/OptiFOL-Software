/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification and implementation for the Parse Error exception
 * @date 2025-05-09
 * @author Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PARSEERROR_HPP
#define PARSEERROR_HPP

#include <stdexcept>

namespace optifol
{

/**
 * @class ParseError
 * @brief An exception indicating some sort of parsing error
 */
class ParseError final : public std::runtime_error
{
public:
    explicit ParseError(const std::string& message):
        std::runtime_error(std::string("Parsing Error: ") + message)
    { }

    explicit ParseError(const std::string& message, const std::size_t):
        std::runtime_error(std::string("Parsing Error: ") + message)
    { }

    explicit ParseError(const char* message, const std::size_t):
        std::runtime_error(std::string("Parsing Error: ") + message)
    { }
};

}

#endif
