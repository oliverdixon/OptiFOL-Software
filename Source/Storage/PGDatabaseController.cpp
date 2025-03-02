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
        project_cache.emplace(*connection);

        pqxx::work tx{*connection};
        tx.exec("SELECT 'init' FROM pg_create_logical_replication_slot($1::text, 'wal2json');",
            pqxx::params{wal_slot_name});
        tx.commit();
    } catch (const pqxx::failure &exception) {
        throw StorageConnectionException(exception.what());
    }

    assert(connection.has_value());
    assert(project_cache.has_value());
}

PGDatabaseController::~PGDatabaseController()
{
    pqxx::work tx{*connection};
    tx.exec("SELECT 'stop' FROM pg_drop_replication_slot($1::text);", pqxx::params{wal_slot_name});
    tx.commit();

    connection->close();
}

void PGDatabaseController::update()
{
    pqxx::work tx{*connection};
    const auto results = tx.exec("SELECT data FROM pg_logical_slot_get_changes($1::text, NULL, " "NULL,"
        "'pretty-print', '0', 'add-msg-prefixes', 'wal2json', 'add-tables', '*.project,*.subsystem', 'include-schemas',"
        "'0', 'include-typmod', '0', 'include-transaction', '0');",
        pqxx::params{wal_slot_name});
    tx.commit();

    if (!results.empty()) {
        for (const auto& update_record : results) {
            const auto json = update_record[0].as<std::string>();
            despatch_json_change(json);
        }

        project_cache->load();
        // TODO: project_cache.reload();
        project_cache->unload();
    }
}

void PGDatabaseController::despatch_json_change(const std::string_view payload)
{
    const auto json_payload = simdjson::padded_string{payload};
    simdjson::ondemand::document document = json_parser.iterate(json_payload);
    auto change_root = document["change"];

    for (auto change : change_root) {
        const auto parsed_payload = change.get<WALJSONPGUpdateNotification>().value();
        if (parsed_payload.scope == WALJSONPGUpdateNotification::Scope::Project)
            switch (parsed_payload.action) {
            case WALJSONPGUpdateNotification::Action::NoOp:
                break;
            case WALJSONPGUpdateNotification::Action::Insert:
                project_cache->enqueue_load(parsed_payload.id);
                break;
            case WALJSONPGUpdateNotification::Action::Update:
                project_cache->enqueue_reload(parsed_payload.id);
                break;
            case WALJSONPGUpdateNotification::Action::Delete:
                project_cache->enqueue_unload(parsed_payload.id);
                break;
            }
    }
}

}
