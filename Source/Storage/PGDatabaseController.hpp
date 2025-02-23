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
     * @post The connection and dependent notification receiver has been established and is maintained in the internal
     *  controller model
     */
    explicit PGDatabaseController(const std::string& db_uri);

    /**
     * @brief Close the database connection.
     * @todo We should close any open transactions here, and flush any changes in the locally cached storage objects.
     */
    ~PGDatabaseController() override;

    void update() override;

private:
    std::optional<pqxx::connection> connection;
    std::optional<PGNotificationReceiver> notification_receiver;

    std::unordered_set<std::unique_ptr<Project>> project_cache;
    std::unordered_set<std::unique_ptr<Subsystem>> subsystem_cache;
};

}

#endif
