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

#include <simdjson.h>
#include <string>
#include <pqxx/connection>

#include "IStorageController.hpp"
#include "PGProjectModel.hpp"
#include "PGSubsystemModel.hpp"
#include "WALJSONPGUpdateNotification.hpp"
#include "../LegacyWrappers.hpp"

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
     * @copybrief IStorageController::update
     * @throws BadStorageNotificationException A propagation update was received from the DB, but it was malformed.
     */
    void update() override;

    const Glib::RefPtr<PGProjectModel> peek_project_model() const override;

    const Glib::RefPtr<PGSubsystemModel> expand_project_model(const Glib::RefPtr<Project>& project) const override;

private:
    /**
     * @brief Parse and action the changes described by a JSON-formatted WAL replication message from the DB
     * @param payload The JSON string posted to the WAL slot
     */
    void despatch_json_change(std::string_view payload);

    void handle_project_change(const WALJSONPGUpdateNotification& notification) const;

    void handle_subsystem_change(const WALJSONPGUpdateNotification& notification);

    const std::string wal_slot_name{"optifol_" + std::to_string(LegacyWrappers::get_pid())};

    simdjson::ondemand::parser json_parser;

    std::optional<pqxx::connection> connection;

    Glib::RefPtr<PGProjectModel> project_model;
};

}

#endif
