/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the generic PostgreSQL NOTIFY--LISTEN receiver.
 * @author Oliver Dixon
 * @date 2025-02-23
 * @version Development
 */

#include <iostream>

#include "PGNotificationReceiver.hpp"

namespace optifol
{
PGNotificationReceiver::PGNotificationReceiver(pqxx::connection &connection,
                                               const std::string_view target_entity,
                                               Action action) :
    pqxx::notification_receiver(connection, construct_channel_name(target_entity, action))
{
}

void PGNotificationReceiver::operator()(const std::string &payload, const int backend_pid)
{
    std::cout <<
        "Notification received from PID " << std::to_string(backend_pid) <<
        " on channel '" << channel() <<
        "' with payload '"<< payload << '\'' <<
    std::endl; // TODO: this is just for testing; we need to flush here for GTK.
}

std::string PGNotificationReceiver::construct_channel_name(const std::string_view entity, const Action action)
{
    std::string action_string;

    switch (action) {
    case Action::Insert:
        action_string = "_update";
        break;
    case Action::Update:
        action_string = "_update";
        break;
    case Action::Delete:
        action_string = "_delete";
        break;
    }

    return std::string(entity) + action_string;
}

}
