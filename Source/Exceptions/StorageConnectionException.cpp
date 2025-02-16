/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation of the storage connection exception
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#include "StorageConnectionException.hpp"

namespace optifol
{

const std::string StorageConnectionException::prefix = "Connection Error: ";

StorageConnectionException::StorageConnectionException(const std::string &message):
    StorageExceptionBase(prefix + message)
{
}

StorageConnectionException::StorageConnectionException(const char *message):
    StorageExceptionBase(prefix + message)
{
}

}
