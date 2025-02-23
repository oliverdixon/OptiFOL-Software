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

#include "IStorageController.hpp"
#include "PGNotificationReceiver.hpp"
#include "Project.hpp"
#include "Subsystem.hpp"

namespace optifol
{

/**
 * @class PGDatabaseController
 * @brief The PGDatabaseController provides a PostgreSQL-based backend storage engine for OptiFOL
 */
class PGDatabaseController :
        public IStorageController
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
     */
    ~PGDatabaseController() override;

    void update() override;

private:
    /**
     * @class StorageContainer
     * @brief A StorageContainer is a PG-specific convenient container within which a class of stored objects,
     *  identifiable by their C++ type, may be maintained.
     * @tparam ConcreteType The concrete storable object type maintained by the container
     * @todo How can we reduce the coupling here, such that it's not dependent on the PGNotificationReceiver?
     */
    template<class ConcreteType>
        requires std::derived_from<ConcreteType, IStorageObject>
    struct StorageContainer
    {
        StorageContainer(pqxx::connection& connection, const std::string& entity_name):
            insert_rx(connection, entity_name, PGNotificationReceiver::Action::Insert),
            update_rx(connection, entity_name, PGNotificationReceiver::Action::Update),
            delete_rx(connection, entity_name, PGNotificationReceiver::Action::Delete)
        { }

        std::unordered_set<std::unique_ptr<ConcreteType>> cache;

        const PGNotificationReceiver insert_rx;
        const PGNotificationReceiver update_rx;
        const PGNotificationReceiver delete_rx;
    };

    std::optional<pqxx::connection> connection;
    std::optional<StorageContainer<Project>> project_container;
    std::optional<StorageContainer<Subsystem>> subsystem_container;
};

}

#endif
