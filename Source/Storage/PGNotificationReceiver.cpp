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

PGNotificationReceiver::PGNotificationReceiver(pqxx::connection &connection, const std::string &channel) :
    pqxx::notification_receiver(connection, channel)
{
}

void PGNotificationReceiver::operator()(const std::string &payload, const int backend_pid)
{
    std::cout << "Notification received from PID " << std::to_string(backend_pid) << ": "<< payload << std::endl;
}

}
