/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef BADSTORAGENOTIFICATIONEXCEPTION_HPP
#define BADSTORAGENOTIFICATIONEXCEPTION_HPP

#include "StorageExceptionBase.hpp"

namespace optifol
{

class BadStorageNotificationException :
        public StorageExceptionBase
{
public:
    explicit BadStorageNotificationException(const std::string &message);

    explicit BadStorageNotificationException(const char *message);

private:
    static const std::string prefix;
};

}

#endif
