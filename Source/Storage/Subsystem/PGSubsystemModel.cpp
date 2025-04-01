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
#include "../PGChronoType.hpp"

namespace optifol
{

PGSubsystemModel::PGSubsystemModel(pqxx::connection &connection) : PGStorableObjectModelBase(connection)
{
}

std::size_t PGSubsystemModel::get_item_count() const noexcept
{
    return subsystems.size();
}

void PGSubsystemModel::register_subsystem(Glib::RefPtr<Subsystem> &&subsystem)
{
    subsystems.insert(std::move(subsystem));
}

Glib::RefPtr<Subsystem> PGSubsystemModel::get_subsystem(const Subsystem &subsystem)
{
    const auto it = subsystems.find(subsystem);
    if (it == subsystems.cend())
        return {};

    return *it;
}

Glib::RefPtr<Subsystem> PGSubsystemModel::get_subsystem(const std::size_t subsystem_id)
{
    const auto it = subsystems.find(subsystem_id);
    if (it == subsystems.cend())
        return {};

    return *it;
}

void PGSubsystemModel::remove_subsystem(const Subsystem &subsystem)
{
    const auto it = subsystems.find(subsystem);
    if (it != subsystems.cend())
        subsystems.erase(it);
}

void PGSubsystemModel::remove_subsystem(const std::size_t subsystem_id)
{
    const auto it = subsystems.find(subsystem_id);
    if (it != subsystems.cend())
        subsystems.erase(it);
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
    register_subsystem(Glib::make_refptr_for_instance(new Subsystem(
        row[0].as<std::size_t>(),
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>()
    )));
}

void PGSubsystemModel::deplace_object(const std::size_t id)
{
    remove_subsystem(id);
}

}
