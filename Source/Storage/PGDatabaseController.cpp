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
#include "WALJSONPGUpdateNotification.hpp"
#include "../Exceptions/StorageConnectionException.hpp"

namespace optifol
{

PGDatabaseController::PGDatabaseController(const std::string& db_uri)
{
    try {
        connection.emplace(db_uri);

        pqxx::work tx{*connection};
        tx.exec_params("SELECT 'init' FROM pg_create_logical_replication_slot($1::text, 'wal2json');", wal_slot_name);
        tx.commit();
    } catch (const pqxx::failure &exception) {
        throw StorageConnectionException(exception.what());
    }

    assert(connection.has_value());
}

PGDatabaseController::~PGDatabaseController()
{
    pqxx::work tx{*connection};
    tx.exec_params("SELECT 'stop' FROM pg_drop_replication_slot($1::text);", wal_slot_name);
    tx.commit();

    connection->close();
}

void PGDatabaseController::update()
{
    pqxx::work tx{*connection};
    const auto results = tx.exec_params("SELECT data FROM pg_logical_slot_get_changes($1::text, NULL, "
        "NULL, 'pretty-print', '0', 'add-msg-prefixes', 'wal2json');", wal_slot_name);
    tx.commit();

    if (results.empty())
        std::cout << "No updates\n";
    else {
        for (const auto& update_record : results) {
            auto json = update_record[0].as<std::string>();
            despatch_json_change(json);
        }

        load_projects();
        // TODO reload_projects
        unload_projects();
    }
}

void PGDatabaseController::load_projects()
{
    if (project_load_queue.empty())
        return;

    std::ostringstream sql_parameter;
    sql_parameter << '{';

    while (project_load_queue.size() > 1) {
        const auto id = project_load_queue.front();
        project_load_queue.pop();
        sql_parameter << id << ',';
    }

    sql_parameter << project_load_queue.front() << '}';
    project_load_queue.pop();

    pqxx::work tx{*connection};
    const pqxx::result result{tx.exec_params(
        "SELECT * FROM project JOIN UNNEST($1::bigint[]) AS filter(id) ON project.id = filter.id;",
        sql_parameter.str()
    )};

    for (const auto& row : result) {
        const std::size_t id = row[0].as<std::size_t>();
        std::cout << "Loading project " << std::to_string(id) << std::endl;
        project_cache.emplace(id, row[1].as<std::string>());
    }

    tx.commit();
}

void PGDatabaseController::reload_projects()
{
    if (project_reload_queue.empty())
        return;

    throw std::runtime_error("Not implemented.");
}

void PGDatabaseController::unload_projects()
{
    while (!project_unload_queue.empty()) {
        const auto id = project_unload_queue.front();
        project_unload_queue.pop();

        const auto cache_it = project_cache.find(id);
        if (cache_it != project_cache.end()) {
            std::cout << "Unloading project " << std::to_string(id) << std::endl;
            project_cache.erase(cache_it);
        }
    }
}

void PGDatabaseController::despatch_json_change(std::string& payload)
{
    const simdjson::padded_string_view payload_parser = simdjson::pad(payload);
    simdjson::ondemand::document document = json_parser.iterate(payload_parser);
    auto change_root = document["change"];

    for (auto change : change_root) {
        WALJSONPGUpdateNotification parsed_payload;
        change.get<WALJSONPGUpdateNotification>().get(parsed_payload);
        if (parsed_payload.get_scope() == IUpdateNotification::Scope::Project)
            switch (parsed_payload.get_action()) {
            case IUpdateNotification::Action::Insert:
                project_load_queue.push(parsed_payload.get_id());
                break;
            case IUpdateNotification::Action::Update:
                project_reload_queue.push(parsed_payload.get_id());
                break;
            case IUpdateNotification::Action::Delete:
                project_unload_queue.push(parsed_payload.get_id());
                break;
            }
    }
}

}
