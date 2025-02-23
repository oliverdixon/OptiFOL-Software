/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic PostgreSQL NOTIFY--LISTEN receiver.
 * @author Oliver Dixon
 * @date 2025-02-23
 * @version Development
 */

#ifndef PGNOTIFICATIONRECEIVER_HPP
#define PGNOTIFICATIONRECEIVER_HPP

#include <pqxx/connection>
#include <pqxx/notification>

namespace optifol
{

/**
 * @class PGNotificationReceiver
 * @brief Implements a PostgreSQL notification receiver on a specified channel with a simple callback mechanism
 * @example PGDatabaseController.hpp
 */
class PGNotificationReceiver :
        public pqxx::notification_receiver
{
public:
    /**
     * @brief Creates a new notification receiver and registers it on the database to listen for messages on the
     *  specified channel
     * @param connection The PostgreSQL DB connection on which to register the RX
     * @param channel The channel on which to listen
     */
    PGNotificationReceiver(pqxx::connection& connection, const std::string& channel);

    /**
     * @brief Actions the notification at the front of the message queue
     * @param payload The payload received from the PG driver
     * @param backend_pid The PID of the notifying process
     * @todo This should call a std::function specified by the constructor, possibly transforming the payload into an
     *  OptiFOL-defined IR, e.g. inserted cache entries of {ID_1, ..., ID_N}. This requires investigation of the domain
     *  of payload contents.
     */
    void operator()(const std::string& payload, int backend_pid) override;
};

}

#endif
