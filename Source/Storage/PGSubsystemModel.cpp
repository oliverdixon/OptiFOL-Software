/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
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
}

void PGSubsystemModel::load()
{
    /*
     * TODO: this is all replicated from PGProjectModel. The PGStorableObjectModel should collate the queues into string
     *  streams, and the individual PG models (project, subsystem, etc.) should only provide (1) a mechanism the
     *  retrieve the actual objects from the DB, since that query will change per StorableType; and (2) a mechanism to
     *  construct the object that is appended to the Gio::ListStore, as that will again depend on the StorableType. */
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

    // Run the query to filter the queued IDs from the subsystem table
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT * FROM subsystem JOIN UNNEST($1::bigint[]) AS filter(id) ON subsystem.id = filter.id;",
        pqxx::params{ sql_parameter.str() }
    )};

    // Emplace subsystems into the cache for each object returned from the DB
    for (const auto& row : result) {
        const auto id = row[0].as<std::size_t>();
        std::cout << "Loading subsystem " << std::to_string(id) << std::endl;

        append(Glib::make_refptr_for_instance(new Subsystem(
            id,
            row[2].as<std::string>(), // TODO: can we do better than raw numerical indexing here? More semantics please.
            row[3].as<std::chrono::system_clock::time_point>(),
            row[4].as<std::chrono::system_clock::time_point>())
        ));
    }

    tx.commit();
}

void PGSubsystemModel::reload()
{
    // TODO
}

void PGSubsystemModel::unload()
{
    while (!unload_queue.empty()) {
        const auto id = unload_queue.front();
        unload_queue.pop();

        auto [found, position] = find(dummy_base,
            [id](const Glib::RefPtr<const Subsystem>& candidate, const Glib::RefPtr<const Subsystem>& dummy) -> auto
            {
                std::ignore = dummy;
                return candidate->get_controller_id() == id;
            }
        );

        if (found) {
            std::cout << "Unloading subsystem " << std::to_string(id) << std::endl;
            remove(position);
        }
    }
}

}
