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

#include <queue>
#include <simdjson.h>
#include <string>
#include <unordered_set>
#include <pqxx/connection>

#include "IStorageController.hpp"
#include "Project.hpp"
#include "StorageHashFunctor.hpp"

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

    /**
     * @throws BadStorageNotificationException A propagation update was received from the DB, but it was malformed.
     */
    void update() override;

private:
    void load_projects();

    void reload_projects();

    void unload_projects();

    void despatch_json_change(std::string& payload);

    std::string wal_slot_name{"test_slot"}; // TODO just for testing

    simdjson::ondemand::parser json_parser;

    std::optional<pqxx::connection> connection;

    std::unordered_set<Project, StorageHashFunctor<Project>, std::equal_to<>> project_cache;

    std::queue<std::size_t> project_load_queue;
    std::queue<std::size_t> project_reload_queue;
    std::queue<std::size_t> project_unload_queue;
};

}

#endif
