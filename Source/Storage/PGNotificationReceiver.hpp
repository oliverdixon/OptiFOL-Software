/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PGNOTIFICATIONRECEIVER_HPP
#define PGNOTIFICATIONRECEIVER_HPP

#include <pqxx/connection>
#include <pqxx/notification>
#include <sigc++/scoped_connection.h>
#include <sigc++-3.0/sigc++/signal.h>

#include "INotificationReceiver.hpp"
#include "NotificationPayload.hpp"

namespace optifol
{

class PGNotificationReceiver :
        public INotificationReceiver,
        public pqxx::notification_receiver
{
public:
    using signal_signature = void(const NotificationPayload &);

    PGNotificationReceiver(pqxx::connection &connection, std::string_view entity,
                           NotificationPayload::PayloadType payload_type,
                           sigc::slot<signal_signature> &&signal_slot);

    void operator()(const std::string &payload, int backend_pid) override;

private:
    NotificationPayload::PayloadType payload_type;

    sigc::signal<signal_signature> signal;
    sigc::scoped_connection signal_connection; // sigc::scoped_connection provides RAII for signal disconnections

    static std::string construct_channel_name(std::string_view entity, NotificationPayload::PayloadType payload_type);
};

}

#endif
