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

#include <pqxx/pqxx>

#include "PGDatabaseController.hpp"

#include "PGUpdateNotification.hpp"
#include "../Logging.hpp"
#include "../Exceptions/StorageConnectionException.hpp"

namespace optifol
{

PGDatabaseController::PGDatabaseController(const std::string& db_uri)
{
    try {
        connection.emplace(db_uri);
        project_model = Glib::make_refptr_for_instance(new PGProjectModel(*connection));

        pqxx::work tx{*connection};
        tx.exec("SELECT 'init' FROM pg_create_logical_replication_slot($1::text, 'wal2json');",
            pqxx::params{wal_slot_name});
        tx.commit();
    } catch (const pqxx::failure &exception) {
        throw StorageConnectionException(exception.what());
    }

    assert(connection.has_value());
    assert(project_model != nullptr);
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

        project_model->flush_inbound_insert();
        project_model->flush_inbound_update();
        project_model->flush_inbound_delete();
    }
}

const Glib::RefPtr<PGProjectModel> PGDatabaseController::peek_project_model() const
{
    return project_model;
}

void PGDatabaseController::despatch_json_change(const std::string_view payload)
{
    const auto json_payload = simdjson::padded_string{payload};
    simdjson::ondemand::document document = json_parser.iterate(json_payload);
    auto change_root = document["change"];

    for (auto change : change_root) {
        const auto parsed_payload = change.get<PGUpdateNotification>().value();

        switch (parsed_payload.scope) {
        case PGUpdateNotification::Scope::Project:
            handle_project_change(parsed_payload);
            break;

        case PGUpdateNotification::Scope::Subsystem:
            handle_subsystem_change(parsed_payload);
            break;

        case PGUpdateNotification::Scope::Empty:
            LOG4CXX_WARN(Logging::get_logger(), "WAL JSON payload was delivered with an empty scope");
            break;
        }
    }
}

void PGDatabaseController::handle_project_change(const PGUpdateNotification &notification) const
{
    switch (notification.action) {
    case PGUpdateNotification::Action::NoOp: break;
    case PGUpdateNotification::Action::Insert:
        project_model->enqueue_load(notification.id);
        break;
    case PGUpdateNotification::Action::Update:
        project_model->enqueue_reload(notification.id);
        break;
    case PGUpdateNotification::Action::Delete:
        project_model->enqueue_unload(notification.id);
        break;
    }
}

void PGDatabaseController::handle_subsystem_change(const PGUpdateNotification &notification) const
{
    if (!notification.associated_fk.has_value()) {
        LOG4CXX_WARN(Logging::get_logger(), "WAL JSON payload for subsystem change did not reference a master project; "
                                            "ignoring and skipping WAL segment.");
        return;
    }

    switch (notification.action) {
    case PGUpdateNotification::Action::NoOp: break;
    case PGUpdateNotification::Action::Insert:
        project_model->get_subsystem_model(*notification.associated_fk)->enqueue_load(notification.id);
        break;
    case PGUpdateNotification::Action::Update:
        project_model->get_subsystem_model(*notification.associated_fk)->enqueue_reload(notification.id);
        break;
    case PGUpdateNotification::Action::Delete:
        project_model->get_subsystem_model(*notification.associated_fk)->enqueue_unload(notification.id);
        break;
    }
}

}
