/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation of the PostgreSQL storage engine backend
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#include "PGDatabaseController.hpp"
#include "../Exceptions/StorageConnectionException.hpp"

namespace optifol
{

PGDatabaseController::PGDatabaseController(const std::string& db_uri)
{
    try {
        connection.emplace(db_uri);
        project_container.emplace(*connection, "project");
        subsystem_container.emplace(*connection, "subsystem");
    } catch (const pqxx::failure &exception) {
        throw StorageConnectionException(exception.what());
    }

    assert(connection.has_value());
    assert(project_container.has_value());
    assert(subsystem_container.has_value());
}

PGDatabaseController::~PGDatabaseController()
{
    connection->close();
}

void PGDatabaseController::update()
{
    connection->get_notifs();
}

}
