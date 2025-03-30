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

#include <pqxx/pqxx>

#include "PGSubsystemModel.hpp"
#include "PGChronoType.hpp"
#include "PGRequirementModel.hpp"
#include "../Logging.hpp"

namespace optifol
{

PGSubsystemModel::PGSubsystemModel(pqxx::connection &connection) :
    PGStorableObjectModelBase(connection)
{
    assert(get_n_items() == 0);
}

PGSubsystemModel::~PGSubsystemModel()
{
    LOG4CXX_INFO(Logging::get_logger(), "Removing all subsystems from deleted project");
    remove_all();
}

PGSubsystemModel::PGSubsystemModel(pqxx::connection &connection, const Project &initial_project,
                                   const std::size_t initial_cache_limit) :
    PGStorableObjectModelBase(connection)
{
    load_for_project(initial_project, initial_cache_limit);
    assert(get_n_items() <= initial_cache_limit);
}

void PGSubsystemModel::load_for_project(const Project &project, const std::size_t limit)
{
    pqxx::work tx{connection};
    const pqxx::result result{
        tx.exec("SELECT id, name, created_at, last_modified FROM subsystem WHERE project_id = $1 LIMIT $2;",
            pqxx::params{project.get_controller_id(), limit})
    };
    tx.commit();

    for (auto &&row: result)
        PGStorableObjectModelBase::enqueue_load(std::move(row));

    flush_inbound_insert();
}

pqxx::result PGSubsystemModel::filter_objects(const std::ostringstream &sql_parameter,
                                              const std::size_t maximum_return_count) const
{
    pqxx::work tx{connection};
    const pqxx::result result{
        tx.exec(
            "SELECT subsystem.id, name, created_at, last_modified FROM subsystem JOIN UNNEST($1::bigint[]) AS "
            "filter(id) ON subsystem.id = filter.id LIMIT $2;", pqxx::params{sql_parameter.str(), maximum_return_count}
        )
    };

    tx.commit();
    return result;
}

void PGSubsystemModel::emplace_object(const pqxx::row &row)
{
    const auto id = row[0].as<std::size_t>();
    LOG4CXX_INFO(Logging::get_logger(), "Loading subsystem with ID " << std::to_string(id));

    const auto loaded = Glib::make_refptr_for_instance(new Subsystem(
            id,
            row[1].as<std::string>(),
            row[2].as<std::chrono::system_clock::time_point>(),
            row[3].as<std::chrono::system_clock::time_point>())
    );

    append(loaded);
    add_requirement_model(loaded, Glib::make_refptr_for_instance(new PGRequirementModel(connection, *loaded)));
}

void PGSubsystemModel::deplace_object(std::size_t id)
{
    auto [found, position] = find(dummy_base,
        [id](const Glib::RefPtr<const Subsystem> &candidate, const Glib::RefPtr<const Subsystem> &dummy) -> auto
        {
            std::ignore = dummy;
            return candidate->get_controller_id() == id;
        }
    );

    if (found) {
        LOG4CXX_INFO(Logging::get_logger(), "Unloading subsystem with ID " << std::to_string(id));
        remove(position);
    }
}

}
