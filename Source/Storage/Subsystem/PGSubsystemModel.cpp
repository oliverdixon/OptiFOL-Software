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

PGSubsystemModel::PGSubsystemModel(pqxx::connection &connection) :
    PGStorableObjectModelBase(connection)
{
}

std::size_t PGSubsystemModel::get_item_count() const noexcept
{
    return model_contents.size();
}

void PGSubsystemModel::register_object(Glib::RefPtr<Subsystem> &&subsystem)
{
    model_contents.insert(std::move(subsystem));
}

Glib::RefPtr<Subsystem> PGSubsystemModel::get_object(const Subsystem &subsystem)
{
    const auto it = model_contents.find(subsystem);
    if (it == model_contents.cend())
        return {};

    return *it;
}

Glib::RefPtr<Subsystem> PGSubsystemModel::get_object(const std::size_t subsystem_id)
{
    const auto it = model_contents.find(subsystem_id);
    if (it == model_contents.cend())
        return {};

    return *it;
}

void PGSubsystemModel::remove_object(const std::size_t subsystem_id)
{
    const auto it = model_contents.find(subsystem_id);
    if (it != model_contents.cend())
        model_contents.erase(it);
}

void PGSubsystemModel::load_for_project(Glib::RefPtr<Project>&& project)
{
    pqxx::work tx{connection};
    const pqxx::result result{
        tx.exec("SELECT id, project_id, name, created_at, last_modified FROM subsystem WHERE project_id = $1;",
            pqxx::params{project->get_controller_id()})
    };
    tx.commit();

    for (auto &&row: result)
        enqueue_load(std::move(row));

    flush_inbound_insert();
}

pqxx::result PGSubsystemModel::filter_objects(const std::ostringstream &sql_parameter,
                                              const std::size_t maximum_return_count) const
{
    pqxx::work tx{connection};
    const pqxx::result result{
        tx.exec(
            "SELECT subsystem.id, project_id, name, created_at, last_modified FROM subsystem JOIN UNNEST($1::bigint[]) "
            "AS filter(id) ON subsystem.id = filter.id LIMIT $2;",
            pqxx::params{sql_parameter.str(), maximum_return_count}
        )
    };

    tx.commit();
    return result;
}

void PGSubsystemModel::emplace_object(const pqxx::row &row)
{
    auto subsystem = Glib::make_refptr_for_instance(new Subsystem(
        row[DBFieldIdx::ID].as<std::size_t>(),
        row[DBFieldIdx::ProjectID].as<std::size_t>(),
        row[DBFieldIdx::Name].as<std::string>(),
        row[DBFieldIdx::CreatedAt].as<std::chrono::system_clock::time_point>(),
        row[DBFieldIdx::LastModified].as<std::chrono::system_clock::time_point>()
    ));

    auto subsystem_insertion_ref = subsystem;
    register_object(std::move(subsystem));
    inform_insertion(std::move(subsystem_insertion_ref));
}

void PGSubsystemModel::deplace_object(const std::size_t id)
{
    remove_object(id);
    inform_deletion(id);
}

}
