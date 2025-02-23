/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition of the bad storage notification exception
 * @author Oliver Dixon
 * @date 2025-02-23
 * @version Development
 */

#ifndef BADSTORAGENOTIFICATIONEXCEPTION_HPP
#define BADSTORAGENOTIFICATIONEXCEPTION_HPP

#include "StorageExceptionBase.hpp"

namespace optifol
{

/**
 * @class BadStorageNotificationException
 * @brief The bad storage notification exception expresses that an invalid notification was provided by the storage
 *  backend
 * @see INotificationReceiver
 * @see NotificationPayload
 */
class BadStorageNotificationException final :
        public StorageExceptionBase
{
public:
    /**
     * @copydoc StorageExceptionBase::StorageExceptionBase(const std::string&)
     */
    explicit BadStorageNotificationException(const std::string &message):
        StorageExceptionBase(prefix + message)
    { }


    /**
     * @copydoc StorageExceptionBase::StorageExceptionBase(const char *)
     */
    explicit BadStorageNotificationException(const char *message):
        StorageExceptionBase(std::string(prefix) + message)
    { }

private:
    /**
     * @brief The prefix string to prepend to all bad storage notification error messages
     */
    static const char * prefix;
};

}

#endif
