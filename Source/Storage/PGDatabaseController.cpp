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
#include "../Logging.hpp"
#include "../Exceptions/StorageConnectionException.hpp"

namespace optifol
{

PGDatabaseController::PGDatabaseController(const std::string& db_uri)
{
    try {
        connection.emplace(db_uri);

        project_model = std::make_unique<PGProjectModel>(*connection);

        subsystem_model = std::make_unique<PGSubsystemModel>(*connection);
        project_model->add_insert_subscriber(sigc::mem_fun(*subsystem_model, &PGSubsystemModel::load_for_project),
            true);

        requirement_model = std::make_unique<PGRequirementModel>(*connection);
        subsystem_model->add_insert_subscriber(sigc::mem_fun(*requirement_model,
            &PGRequirementModel::load_for_subsystem), true);

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
        "'pretty-print', '0', 'add-msg-prefixes', 'wal2json', 'add-tables', '*.project,*.subsystem,*.requirement', "
        "'include-schemas', '0', 'include-typmod', '0', 'include-transaction', '0');",
        pqxx::params{wal_slot_name});
    tx.commit();

    if (!results.empty()) {
        for (const auto& update_record : results) {
            const auto json = update_record[0].as<std::string>();
            despatch_json_change(json);
        }

        project_model->flush_inbound();
        subsystem_model->flush_inbound();
        requirement_model->flush_inbound();
    }
}

Glib::RefPtr<ProjectHierarchicalModel> PGDatabaseController::build_project_hierarchical_model() const
{
    auto hierarchical_model = Glib::make_refptr_for_instance(new ProjectHierarchicalModel());

    // Set up the two-way synchronisation on the project model
    add_all_listeners(*hierarchical_model);
    const auto project_count = hierarchical_model->get_n_items();
    assert(project_count == project_model->get_item_count());

    // Set up the two-way synchronisation on the subsystem models: one for each project
    for (guint project_idx = 0; project_idx < project_count; ++project_idx) {
        const auto& subsystem_hierarchical_model =
            hierarchical_model->expand_project(*hierarchical_model->get_typed_object<Project>(project_idx));
        add_all_listeners(*subsystem_hierarchical_model);

        // Set up the two-way synchronisation on the requirement models: one for each subsystem
        const auto subsystem_count = subsystem_hierarchical_model->get_n_items();
        for (guint subsystem_idx = 0; subsystem_idx < subsystem_count; ++subsystem_idx)
            add_all_listeners(*subsystem_hierarchical_model->query_object(
                *subsystem_hierarchical_model->get_typed_object<Subsystem>(subsystem_idx)));
    }

    return hierarchical_model;
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
            handle_change<Project>(parsed_payload);
            break;

        case PGUpdateNotification::Scope::Subsystem:
            handle_change<Subsystem>(parsed_payload);
            break;

        case PGUpdateNotification::Scope::Requirement:
            handle_change<Requirement>(parsed_payload);
            break;

        case PGUpdateNotification::Scope::Empty:
            LOG4CXX_WARN(Logging::get_logger(), "WAL JSON payload was delivered with an empty scope");
            break;
        }
    }
}

}
