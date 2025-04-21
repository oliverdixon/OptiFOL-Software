/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief String names for Optifol exception prefixes
 * @note These are required to exist in a distinct translation unit to avoid linker errors
 * @todo There must be a better way to implement a custom chain of exceptions, rather than having to manually specify
 *  exception classes as final!
 * @author Oliver Dixon
 * @date 2025-02-23
 * @version Development
 */

#include "BadStorageNotificationException.hpp"
#include "SemanticException.hpp"
#include "StorageConnectionException.hpp"

namespace optifol
{

const char * StorageExceptionBase::prefix = "Storage Error / ";
const char * ExceptionBase::prefix = "Optifol Error / ";

const char * SemanticException::prefix = "Semantic Exception: ";
const char * StorageConnectionException::prefix = "Connection Error: ";
const char * BadStorageNotificationException::prefix = "Storage Notification Error: ";

}
