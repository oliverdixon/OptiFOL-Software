/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef INOTIFICATIONRECEIVER_HPP
#define INOTIFICATIONRECEIVER_HPP

#include "NotificationPayload.hpp"

namespace optifol
{

class INotificationReceiver
{
public:
    virtual ~INotificationReceiver() = default;

    [[nodiscard]] virtual std::optional<NotificationPayload> consume() = 0;
};

}

#endif
