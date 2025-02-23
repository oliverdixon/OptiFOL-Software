/*
* Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */


#ifndef PGNOTIFICATIONRECEIVER_HPP
#define PGNOTIFICATIONRECEIVER_HPP

#include <forward_list>
#include <pqxx/connection>
#include <pqxx/notification>

#include "INotificationReceiver.hpp"

namespace optifol
{

class PGNotificationReceiver :
        public INotificationReceiver,
        public pqxx::notification_receiver
{
public:
    PGNotificationReceiver(pqxx::connection& connection, std::string_view entity,
        NotificationPayload::PayloadType payload_type);

    void operator()(const std::string& payload, int backend_pid) override;

    [[nodiscard]] std::optional<NotificationPayload> consume() override;

private:
    NotificationPayload::PayloadType payload_type;

    std::forward_list<NotificationPayload> payloads;

    static std::string construct_channel_name(std::string_view entity, NotificationPayload::PayloadType payload_type);
};

}

#endif
