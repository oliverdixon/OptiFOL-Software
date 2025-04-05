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
        "'pretty-print', '0', 'add-msg-prefixes', 'wal2json', 'add-tables', '*.project,*.subsystem', 'include-schemas',"
        "'0', 'include-typmod', '0', 'include-transaction', '0');",
        pqxx::params{wal_slot_name});
    tx.commit();

    // TODO restore
}

Glib::RefPtr<ProjectHierarchicalModel> PGDatabaseController::build_project_hierarchical_model() const
{
    auto hierarchical_model = Glib::make_refptr_for_instance(new ProjectHierarchicalModel());

    // Set up the two-way insertion synchronisation on the project
    project_model->add_insert_subscriber(sigc::mem_fun(*hierarchical_model,
        &ProjectHierarchicalModel::register_object), true);
    hierarchical_model->add_insert_subscriber(sigc::mem_fun(*project_model,
        &PGProjectModel::register_object), false);

    const auto project_count = hierarchical_model->get_n_items();
    assert(project_count == project_model->get_item_count());

    // Set up the two-way insertion synchronisation on the subsystem models: one for each project
    for (guint project_idx = 0; project_idx < project_count; ++project_idx) {
        const auto& subsystem_hierarchical_model =
            hierarchical_model->expand_project(*hierarchical_model->get_typed_object<Project>(project_idx));

        subsystem_model->add_insert_subscriber(sigc::mem_fun(*subsystem_hierarchical_model,
            &SubsystemHierarchicalModel::register_object), true);
        subsystem_hierarchical_model->add_insert_subscriber(sigc::mem_fun(*subsystem_model,
            &PGSubsystemModel::register_object), false);

        // Set up the two-way insertion synchronisation on the requirement models: one for each subsystem
        const auto subsystem_count = subsystem_hierarchical_model->get_n_items();
        for (guint subsystem_idx = 0; subsystem_idx < subsystem_count; ++subsystem_idx) {
            const auto& requirement_hierarchical_model =
                subsystem_hierarchical_model->query_object(*subsystem_hierarchical_model->
                    get_typed_object<Subsystem>(subsystem_idx));

            requirement_model->add_insert_subscriber(sigc::mem_fun(*requirement_hierarchical_model,
                &RequirementHierarchicalModel::register_object), true);
            requirement_hierarchical_model->add_insert_subscriber(sigc::mem_fun(*requirement_model,
                &PGRequirementModel::register_object), false);
        }
    }

    return hierarchical_model;
}

}
