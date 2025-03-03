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
    for (const auto& row : result)
        emplace_project(row);

    assert(get_n_items() <= initial_cache_limit);
}

void PGProjectModel::load()
{
    if (load_queue.empty())
        return;

    // Build the queued IDs into a stream for substitution into the SQL query
    std::ostringstream sql_parameter;
    sql_parameter << '{';

    while (load_queue.size() > 1) {
        const auto id = load_queue.front();
        load_queue.pop();
        sql_parameter << id << ',';
    }

    sql_parameter << load_queue.front() << '}';
    load_queue.pop();

    // Run the query to filter the queued IDs from the project table
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT * FROM project JOIN UNNEST($1::bigint[]) AS filter(id) ON project.id = filter.id;",
        pqxx::params{ sql_parameter.str() }
    )};

    // Emplace projects into the cache for each object returned from the DB
    for (const auto& row : result)
        emplace_project(row);

    tx.commit();
}

void PGProjectModel::reload()
{
    // TODO
}

void PGProjectModel::unload()
{
    while (!unload_queue.empty()) {
        const auto id = unload_queue.front();
        unload_queue.pop();

        auto [found, position] = find(dummy_base,
            [id](const Glib::RefPtr<const Project>& candidate, const Glib::RefPtr<const Project>& dummy) -> auto
            {
                std::ignore = dummy;
                return candidate->get_controller_id() == id;
            }
        );

        if (found) {
            std::cout << "Unloading project " << std::to_string(id) << std::endl;
            remove(position);
        }
    }
}

void PGProjectModel::emplace_project(const pqxx::row &row)
{
    const auto id = row[0].as<std::size_t>();
    std::cout << "Loading project " << std::to_string(id) << std::endl;

    append(Glib::make_refptr_for_instance(new Project(
        id,
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>())
    ));
}

}
