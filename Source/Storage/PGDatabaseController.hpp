/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the PostgreSQL storage engine backend
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#ifndef PGDATABASECONTROLLER_HPP
#define PGDATABASECONTROLLER_HPP

#include <string>
#include <unordered_set>
#include <sigc++-3.0/sigc++/trackable.h>

#include "IStorageController.hpp"
#include "PGNotificationReceiver.hpp"
#include "Project.hpp"
#include "StorageHashFunctor.hpp"

namespace optifol
{

/**
 * @class PGDatabaseController
 * @brief The PGDatabaseController provides a PostgreSQL-based backend storage engine for OptiFOL
 */
class PGDatabaseController :
        public IStorageController,
        public sigc::trackable
{
public:
    /**
     * @brief Establish a connection with the database at the given URI.
     * @param db_uri URI of the PostgreSQL database, beginning with the 'postgresql://' protocol specifier.
     * @throws StorageConnectionException The connection could not be successfully established
     * @post The connection and all dependent concrete containers have been established and is maintained in the
     *  internal controller model
     */
    explicit PGDatabaseController(const std::string& db_uri);

    /**
     * @brief Close the database connection.
     * @todo We should close any open transactions here, and flush any changes in the locally cached storage objects.
     * @note This hides the non-virtual ~sigc::trackable destructor, but it is not a problem in this case, as we would
     *  never refer to a PGDatabaseController instance as a pointer to sigc::trackable.
     */
    // ReSharper disable once CppHidingFunction
    ~PGDatabaseController() override;

    void update() override;

    void load_project(const NotificationPayload& payload) override;

    void reload_project(const NotificationPayload& payload) override;

    void unload_project(const NotificationPayload& payload) override;

private:
    template<StorableType Type>
    struct Container
    {
        Container(pqxx::connection& connection,
                  const std::string_view entity,
                  sigc::slot<PGNotificationReceiver::signal_signature>&& insert_slot,
                  sigc::slot<PGNotificationReceiver::signal_signature>&& update_slot,
                  sigc::slot<PGNotificationReceiver::signal_signature>&& delete_slot):
            insert_rx(connection, entity, NotificationPayload::PayloadType::Insert, std::move(insert_slot)),
            update_rx(connection, entity, NotificationPayload::PayloadType::Update, std::move(update_slot)),
            delete_rx(connection, entity, NotificationPayload::PayloadType::Delete, std::move(delete_slot))
        { }

        std::unordered_set<Type, StorageHashFunctor<Type>, std::equal_to<>> cache;

    private:
        PGNotificationReceiver insert_rx;
        PGNotificationReceiver update_rx;
        PGNotificationReceiver delete_rx;
    };

    std::optional<pqxx::connection> connection;

    std::optional<Container<Project>> project_container;
};

}

#endif
