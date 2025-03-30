/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the PostgreSQL-backed requirement storage container
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#include <pqxx/pqxx>

#include "PGRequirementModel.hpp"
#include "PGChronoType.hpp"

namespace optifol
{

PGRequirementModel::PGRequirementModel(pqxx::connection &connection) :
    PGStorableObjectModel(connection)
{
    assert(get_n_items() == 0);
}

PGRequirementModel::PGRequirementModel(pqxx::connection &connection, const Subsystem &initial_subsystem,
        const std::size_t initial_cache_limit):
    PGStorableObjectModel(connection)
{
    load_for_subsystem(initial_subsystem, initial_cache_limit);
    assert(get_n_items() <= initial_cache_limit);
}

void PGRequirementModel::load_for_subsystem(const Subsystem &subsystem, std::size_t limit)
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
            "SELECT requirement.id, name, created_at, last_modified, sentence, priority, description, test_id "
            "FROM requirement WHERE subsystem_id = $1 LIMIT $2;",
            pqxx::params{ subsystem.get_controller_id(), limit }
    )};

    tx.commit();

    for (auto &&row: result)
        enqueue_load(std::move(row));

    flush_inbound_insert();
}

pqxx::result PGRequirementModel::filter_objects(const std::ostringstream &sql_parameter,
                                                const std::size_t maximum_return_count) const
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT requirement.id, name, created_at, last_modified, sentence, priority, description, test_id "
        "FROM requirement JOIN UNNEST($1::bigint[]) AS filter(id) ON requirement.id = filter.id LIMIT $2;",
        pqxx::params{ sql_parameter.str(), maximum_return_count }
    )};

    tx.commit();
    return result;
}

void PGRequirementModel::emplace_object(const pqxx::row &row)
{
    // TODO: need a better way of matching fields to indexes
    const auto id = row[0].as<std::size_t>();
    std::optional<std::size_t> test_id;
    if (!row[7].is_null())
        test_id.emplace(row[7].as<std::size_t>());

    append(Glib::make_refptr_for_instance(new Requirement(
        id,
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>(),
        row[4].as<std::string>(),
        row[5].as<std::size_t>(),
        row[6].as<std::string>(),
        test_id,
        0) // TODO: stakeholder
    ));
}

void PGRequirementModel::deplace_object(const std::size_t id)
{
    std::ignore = id;
}

}
