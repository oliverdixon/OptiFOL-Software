/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include <iostream>
#include <pqxx/pqxx>

#include "PGProjectCache.hpp"
#include "PGChronoType.hpp"

namespace optifol
{

PGProjectCache::PGProjectCache(pqxx::connection &connection):
    PGStorableObjectCacheBase(connection)
{
}

void PGProjectCache::load()
{
    if (load_queue.empty())
        return;

    std::ostringstream sql_parameter;
    sql_parameter << '{';

    while (load_queue.size() > 1) {
        const auto id = load_queue.front();
        load_queue.pop();
        sql_parameter << id << ',';
    }

    sql_parameter << load_queue.front() << '}';
    load_queue.pop();

    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT * FROM project JOIN UNNEST($1::bigint[]) AS filter(id) ON project.id = filter.id;",
        pqxx::params{ sql_parameter.str() }
    )};

    for (const auto& row : result) {
        const auto id = row[0].as<std::size_t>();
        std::cout << "Loading project " << std::to_string(id) << std::endl;

        cache.emplace(id,
            row[1].as<std::string>(),
            row[2].as<std::chrono::system_clock::time_point>(),
            row[3].as<std::chrono::system_clock::time_point>()
        );
    }

    tx.commit();
}

}
