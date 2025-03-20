/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation of the PostgreSQL storable object model base
 * @author Oliver Dixon
 * @date 2025-03-20
 * @version Development
 */

#include <pqxx/pqxx>

#include "PGStorableObjectModelBase.hpp"

namespace optifol
{

void PGStorableObjectModelBase::enqueue_load(std::size_t id)
{
    load_queue.push(id);
}

void PGStorableObjectModelBase::enqueue_reload(std::size_t id)
{
    reload_queue.push(id);
}

void PGStorableObjectModelBase::enqueue_unload(std::size_t id)
{
    unload_queue.push(id);
}

void PGStorableObjectModelBase::enqueue_load(pqxx::row &&row)
{
    load_queue.push(row);
}

void PGStorableObjectModelBase::enqueue_reload(pqxx::row &&row)
{
    reload_queue.push(row);
}

void PGStorableObjectModelBase::enqueue_unload(pqxx::row &&row)
{
    unload_queue.push(row);
}

void PGStorableObjectModelBase::pq_load()
{
    if (load_queue.empty())
        return;

    // Build the queued IDs into a stream for substitution into the SQL query
    std::ostringstream sql_parameter;
    std::size_t sql_parameter_count = 0;

    sql_parameter << '{';

    while (!load_queue.empty()) {
        /*
         * If we have a raw ID, build it into the stream to be fetched from the DB. If we have a prefetched row, emplace
         * it immediately.
         */

        std::visit([this, &sql_parameter, &sql_parameter_count](auto&& arg)
        {
            using DeducedType = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<DeducedType, std::size_t>) {
                sql_parameter << arg << ',';
                ++sql_parameter_count;
            } else if constexpr (std::is_same_v<DeducedType, pqxx::row>)
                emplace_object(arg);
        }, load_queue.front());

        load_queue.pop();
    }

    if (sql_parameter_count > 0) {
        // Remove the trailing delimeter from the stream
        sql_parameter.seekp(-1, std::ios_base::end);
        sql_parameter << '}';

        // Run the query to filter the queued IDs from the DB object table, and load into the cache
        const auto db_result = filter_objects(sql_parameter);
        for (const auto& row : db_result)
            emplace_object(row);
    }
}

void PGStorableObjectModelBase::pq_reload()
{
    // TODO implement
}

void PGStorableObjectModelBase::pq_unload()
{
    // TODO implement
}

}
