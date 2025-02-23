/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef NOTIFICATIONPAYLOAD_HPP
#define NOTIFICATIONPAYLOAD_HPP

#include "IStorageObject.hpp"

namespace optifol
{

struct NotificationPayload
{
    enum class PayloadType
    {
        Insert,
        Update,
        Delete
    };

    explicit NotificationPayload(const std::size_t id):
        id(id)
    {
    }

    std::size_t id;
};

}

#endif
