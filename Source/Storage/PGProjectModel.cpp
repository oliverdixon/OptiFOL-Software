/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the PostgreSQL-backed project storage container
 * @author Oliver Dixon
 * @date 2025-02-28
 * @version Development
 */

#include <iostream>
#include <pqxx/pqxx>

#include "PGProjectModel.hpp"
#include "PGChronoType.hpp"

namespace optifol
{

PGProjectModel::PGProjectModel(pqxx::connection &connection, std::size_t initial_cache_limit):
    PGStorableObjectModel(connection)
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec("SELECT id, name, created_at, last_modified FROM project ORDER BY last_modified "
                                      "DESC LIMIT $1;", pqxx::params{initial_cache_limit})};
    tx.commit();

    for (const auto& row : result)
        PGProjectModel::emplace_object(row);

    assert(get_n_items() <= initial_cache_limit);
}

Glib::RefPtr<PGSubsystemModel> PGProjectModel::get_subsystem_model(const Glib::RefPtr<Project> &project) const
{
    return subsystem_models.find(project)->second; // TODO safety
}

pqxx::result PGProjectModel::filter_objects(const std::ostringstream& sql_parameter) const
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT * FROM project JOIN UNNEST($1::bigint[]) AS filter(id) ON project.id = filter.id;",
        pqxx::params{ sql_parameter.str() }
    )}; // TODO: should we have a LIMIT clause here?

    tx.commit();
    return result; // TODO: ensure copy elision
}

void PGProjectModel::emplace_object(const pqxx::row &row)
{
    const auto id = row[0].as<std::size_t>();
    std::cout << "Loading project " << std::to_string(id) << std::endl;

    auto loaded = Glib::make_refptr_for_instance(new Project(
        id,
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>())
    );

    /*
     * Append the loaded project to ourselves (since we are a Gio::ListModel), and provide shared ownership with the
     * subsystem such that a slot for a subsystem model is associated with the master project.
     */
    append(loaded);
    subsystem_models.emplace(loaded, Glib::make_refptr_for_instance(new PGSubsystemModel(connection, *loaded)));
}

}
