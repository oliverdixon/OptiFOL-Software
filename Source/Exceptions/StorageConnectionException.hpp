/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the storage connection exception
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#ifndef STORAGECONNECTIONEXCEPTION_HPP
#define STORAGECONNECTIONEXCEPTION_HPP

#include "StorageExceptionBase.hpp"

namespace optifol
{

/**
 * @class StorageConnectionException
 * @brief Represents an exception involving the initial or ongoing basic connection to the storage engine
 */
class StorageConnectionException final :
        public StorageExceptionBase
{
public:
    /**
     * @copydoc StorageExceptionBase::StorageExceptionBase(const std::string&)
     */
    explicit StorageConnectionException(const std::string& message);

    /**
     * @copydoc StorageExceptionBase::StorageExceptionBase(const char *)
     */
    explicit StorageConnectionException(const char* message);

private:
    /**
     * @brief The prefix string to prepend to all storage connection error messages
     */
    static const std::string prefix;
};

}

#endif
