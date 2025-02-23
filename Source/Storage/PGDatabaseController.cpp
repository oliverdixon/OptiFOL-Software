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

#include "PGDatabaseController.hpp"
#include "../Exceptions/StorageConnectionException.hpp"

namespace optifol
{

PGDatabaseController::PGDatabaseController(const std::string& db_uri)
{
    try {
        connection.emplace(db_uri);
        project_insert_rx.emplace(*connection, "project", NotificationPayload::PayloadType::Insert);
        project_update_rx.emplace(*connection, "project", NotificationPayload::PayloadType::Update);
        project_delete_rx.emplace(*connection, "project", NotificationPayload::PayloadType::Delete);
    } catch (const pqxx::failure &exception) {
        throw StorageConnectionException(exception.what());
    }

    assert(connection.has_value());
    assert(project_insert_rx.has_value());
    assert(project_update_rx.has_value());
    assert(project_delete_rx.has_value());
}

PGDatabaseController::~PGDatabaseController()
{
    connection->close();
}

void PGDatabaseController::update()
{
    connection->get_notifs();

    while (const std::optional<NotificationPayload> payload = project_insert_rx->consume())
        load_project(payload->id);

    while (const std::optional<NotificationPayload> payload = project_update_rx->consume())
        reload_project(payload->id);

    while (const std::optional<NotificationPayload> payload = project_delete_rx->consume())
        unload_project(payload->id);
}

void PGDatabaseController::load_project(const std::size_t id)
{
    std::cout << "Loading project " << std::to_string(id) << std::endl;

    pqxx::work tx{*connection};
    const auto results{tx.exec("SELECT id, name FROM project WHERE id = " + std::to_string(id) + ';' )};

    for (const auto& row : results)
        project_cache.emplace(
            row[0].as<std::size_t>(),
            row[1].as<std::string>()
        );

    tx.commit();
}

void PGDatabaseController::reload_project(const std::size_t id)
{
    std::cout << "Reloading project " << std::to_string(id) << std::endl;

    unload_project(id);
    load_project(id);
}

void PGDatabaseController::unload_project(const std::size_t id)
{
    std::cout << "Attempting to unload project " << std::to_string(id) << std::endl;
    const auto cache_it = project_cache.find(id);

    if (cache_it != project_cache.end()) {
        std::cout << "Unloading project " << std::to_string(id) << std::endl;
        project_cache.erase(cache_it);
    }
}

}
