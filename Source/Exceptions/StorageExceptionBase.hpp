/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition of base for storage exceptions
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#ifndef STORAGEEXCEPTIONBASE_HPP
#define STORAGEEXCEPTIONBASE_HPP

#include "ExceptionBase.hpp"

namespace optifol
{

/**
 * @class StorageExceptionBase
 * @brief Provides a common base for all runtime errors raised by a storage backend engine
 */
class StorageExceptionBase :
        public ExceptionBase
{
public:
    /**
     * @copydoc ExceptionBase::ExceptionBase(const std::string&)
     */
    explicit StorageExceptionBase(const std::string& message):
        ExceptionBase(prefix + message)
    { }

    /**
     * @copydoc ExceptionBase::ExceptionBase(const char *)
     */
    explicit StorageExceptionBase(const char* message):
        ExceptionBase(std::string(prefix) + message)
    { }

private:
    /**
     * @brief The prefix string to prepend to all Storage Exception messages
     */
    static const char * prefix;
};

}

#endif
