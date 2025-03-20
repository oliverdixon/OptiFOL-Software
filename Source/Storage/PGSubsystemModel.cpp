/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the PostgreSQL-backed subsystem storage container
 * @author Oliver Dixon
 * @date 2025-03-18
 * @version Development
 */

#include <iostream>
#include <pqxx/pqxx>

#include "PGSubsystemModel.hpp"
#include "PGChronoType.hpp"

namespace optifol
{

PGSubsystemModel::PGSubsystemModel(pqxx::connection &connection) :
    PGStorableObjectModel(connection)
{
    assert(get_n_items() == 0);
}

PGSubsystemModel::PGSubsystemModel(pqxx::connection &connection, const Project & initial_project,
        const std::size_t initial_cache_limit) :
    PGStorableObjectModel(connection)
{
    load_for_project(initial_project, initial_cache_limit);
    assert(get_n_items() <= initial_cache_limit);
}

void PGSubsystemModel::load_for_project(const Project &project, const std::size_t limit)
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec("SELECT id, name, created_at, last_modified FROM subsystem WHERE "
                                      "project_id = $1 LIMIT $2;",
                                      pqxx::params{ project.get_controller_id(), limit } )};
    tx.commit();

    for (auto&& row : result)
        enqueue_load(std::move(row));

    load();
}

pqxx::result PGSubsystemModel::filter_objects(const std::ostringstream &sql_parameter) const
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT * FROM subsystem JOIN UNNEST($1::bigint[]) AS filter(id) ON subsystem.id = filter.id;",
        pqxx::params{ sql_parameter.str() }
    )}; // TODO: should we have a LIMIT clause here?

    tx.commit();
    return result; // TODO: ensure copy elision
}

void PGSubsystemModel::emplace_object(const pqxx::row &row)
{
    const auto id = row[0].as<std::size_t>();
    std::cout << "Loading subsystem " << std::to_string(id) << std::endl;

    append(Glib::make_refptr_for_instance(new Subsystem(
        id,
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>())
    ));
}

}
