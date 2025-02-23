/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition for the base OptiFOL C++ exception
 * @author Oliver Dixon
 * @date 2025-02-09
 * @version Development
 */

#ifndef EXCEPTIONBASE_HPP
#define EXCEPTIONBASE_HPP

#include <stdexcept>

namespace optifol
{

/**
 * @class ExceptionBase
 * @brief The ExceptionBase is used by all OptiFOL C++ exception classes as a base
 */
class ExceptionBase:
    public std::runtime_error
{
public:
    /**
     * @brief Construct a new OptiFOL exception from a C++ string message.
     * @param message Message describing the cause of the exception
     */
    explicit ExceptionBase(const std::string& message):
        std::runtime_error(prefix + message)
    { }

    /**
     * @brief Construct a new SemanticException from a C-style string message.
     * @param message Message describing the cause of the SemanticException
     */
    explicit ExceptionBase(const char* message):
        std::runtime_error(std::string(prefix) + message)
    { }

private:
    /**
     * @brief The prefix string to prepend to all OptiFOL exception messages
     */
    static const char * prefix;
};

}

#endif
