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

#include <iostream>
#include <pqxx/pqxx>
#include <sigc++-3.0/sigc++/functors/mem_fun.h>

#include "PGDatabaseController.hpp"
#include "../Exceptions/StorageConnectionException.hpp"

namespace optifol
{

PGDatabaseController::PGDatabaseController(const std::string& db_uri)
{
    try {
        connection.emplace(db_uri);
        project_container.emplace(
            *connection, "project",
            sigc::mem_fun(*this, &PGDatabaseController::load_project),
            sigc::mem_fun(*this, &PGDatabaseController::reload_project),
            sigc::mem_fun(*this, &PGDatabaseController::unload_project)
        );
    } catch (const pqxx::failure &exception) {
        throw StorageConnectionException(exception.what());
    }

    assert(connection.has_value());
}

PGDatabaseController::~PGDatabaseController()
{
    connection->close();
}

void PGDatabaseController::update()
{
    connection->get_notifs();
}

void PGDatabaseController::load_project(const NotificationPayload& payload)
{
    std::cout << "Loading project " << std::to_string(payload.id) << std::endl;

    // TODO: clearly, this is highly inefficient. It's just for testing.
    pqxx::work tx{*connection};
    const auto results{tx.exec("SELECT id, name FROM project WHERE id = " + std::to_string(payload.id) + ';' )};

    for (const auto& row : results)
        project_container->cache.emplace(
            row[0].as<std::size_t>(),
            row[1].as<std::string>()
        );

    tx.commit();
}

void PGDatabaseController::reload_project(const NotificationPayload& payload)
{
    std::cout << "Reloading project " << std::to_string(payload.id) << std::endl;

    unload_project(payload);
    load_project(payload);
}

void PGDatabaseController::unload_project(const NotificationPayload& payload)
{
    std::cout << "Attempting to unload project " << std::to_string(payload.id) << std::endl;

    auto& cache = project_container->cache;
    const auto cache_it = cache.find(payload.id);

    if (cache_it != cache.end()) {
        std::cout << "Unloading project " << std::to_string(payload.id) << std::endl;
        cache.erase(cache_it);
    }
}

}
