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
#include "../LegacyWrappers.hpp"
#include "Project/PGProjectModel.hpp"
#include "Project/ProjectHierarchicalModel.hpp"
#include "Requirement/PGRequirementModel.hpp"
#include "Subsystem/PGSubsystemModel.hpp"

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

    [[nodiscard]] Glib::RefPtr<ProjectHierarchicalModel> build_project_hierarchical_model() const override;

private:
    const std::string wal_slot_name{"optifol_" + std::to_string(LegacyWrappers::get_pid())};

    simdjson::ondemand::parser json_parser;

    std::optional<pqxx::connection> connection;

    std::unique_ptr<PGProjectModel> project_model;

    std::unique_ptr<PGSubsystemModel> subsystem_model;

    std::unique_ptr<PGRequirementModel> requirement_model;
};

}

#endif
