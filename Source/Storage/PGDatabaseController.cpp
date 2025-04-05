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
    auto model = Glib::make_refptr_for_instance(new ProjectHierarchicalModel());
    project_model->add_insert_subscriber(sigc::mem_fun(*model, &ProjectHierarchicalModel::register_project));
    return model;
}

}
