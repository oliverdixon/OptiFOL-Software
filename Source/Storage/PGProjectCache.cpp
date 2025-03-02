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

#include "PGProjectCache.hpp"
#include "PGChronoType.hpp"

namespace optifol
{

PGProjectCache::PGProjectCache(pqxx::connection &connection, std::size_t initial_cache_limit):
    PGStorableObjectCacheBase(connection)
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec("SELECT id, name, created_at, last_modified FROM project ORDER BY last_modified "
                                      "DESC LIMIT $1;", pqxx::params{initial_cache_limit})};
    for (const auto& row : result)
        emplace_project(row);

    assert(project_model->get_n_items() <= initial_cache_limit);
}

void PGProjectCache::load()
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

void PGProjectCache::unload()
{
    while (!unload_queue.empty()) {
        const auto id = unload_queue.front();
        unload_queue.pop();

        // TODO tomorrow: transparent lookup with search functor
        auto [found, position] = project_model->find(Glib::make_refptr_for_instance(new Project(id, "anything")),
            [](const Glib::RefPtr<const Project>& lhs, const Glib::RefPtr<const Project>& rhs) -> auto { return lhs->operator==(*rhs); });
        if (found) {
            std::cout << "Unloading project " << std::to_string(id) << std::endl;
            project_model->remove(position);
        }
    }
}

void PGProjectCache::emplace_project(const pqxx::row &row)
{
    const auto id = row[0].as<std::size_t>();
    std::cout << "Loading project " << std::to_string(id) << std::endl;

    project_model->append(Glib::make_refptr_for_instance(new Project(
        id,
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>())
    ));
}

}
