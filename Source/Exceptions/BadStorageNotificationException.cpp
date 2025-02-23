/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "BadStorageNotificationException.hpp"

namespace optifol
{

const std::string BadStorageNotificationException::prefix = "Storage Notification Error: ";

BadStorageNotificationException::BadStorageNotificationException(const std::string &message):
    StorageExceptionBase(prefix + message)
{
}

BadStorageNotificationException::BadStorageNotificationException(const char *message):
    StorageExceptionBase(prefix + message)
{
}

} // optifol