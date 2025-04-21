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

bool PGSubsystemModel::remove_object(const std::size_t subsystem_id)
{
    const auto it = model_contents.find(subsystem_id);

    if (it != model_contents.cend()) {
        model_contents.erase(it);
        return true;
    }

    return false;
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
        enqueue_inbound_load(std::move(row));

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

void PGSubsystemModel::emplace_inbound_object(const pqxx::row &row)
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

void PGSubsystemModel::update_inbound_object(const pqxx::row &row)
{
    const auto subsystem_idx = row[DBFieldIdx::ID].as<std::size_t>();
    const auto subsystem_it = model_contents.find(subsystem_idx);

    if (subsystem_it == model_contents.end())
        emplace_inbound_object(row);
    else {
        const auto& subsystem = *subsystem_it;
        bool changed = false;

        const auto name = row[DBFieldIdx::Name].as<std::string_view>();
        const auto project_tag = row[DBFieldIdx::ProjectID].as<std::size_t>();
        const auto creation_time = row[DBFieldIdx::CreatedAt].as<std::chrono::system_clock::time_point>();
        const auto modified_time = row[DBFieldIdx::LastModified].as<std::chrono::system_clock::time_point>();

        if (subsystem->get_identifier() != name) {
            subsystem->set_identifier(std::string(name));
            changed = true;
        }

        if (subsystem->get_relevant_project_tag() != project_tag) {
            subsystem->set_project_tag(project_tag);
            changed = true;
        }

        if (subsystem->get_creation_time() != creation_time) {
            subsystem->set_creation_time(creation_time);
            changed = true;
        }

        if (subsystem->get_modified_time() != modified_time) {
            subsystem->set_modified_time(modified_time);
            changed = true;
        }

        if (changed)
            inform_update(subsystem);
    }
}

void PGSubsystemModel::deplace_inbound_object(const std::size_t id)
{
    if (remove_object(id))
        inform_deletion(id);
}

void PGSubsystemModel::emplace_outbound_object(const Glib::RefPtr<Subsystem> &item, pqxx::work &tx) const
{
    tx.exec("INSERT INTO subsystem (id, project_id, name, created_at, last_modified) VALUES ($1, $2, $3, $4, $5);",
        pqxx::params{ item->get_controller_id(), item->get_relevant_project_tag(), item->get_identifier(),
            item->get_creation_time(), item->get_modified_time() });
}

void PGSubsystemModel::update_outbound_object(const Glib::RefPtr<Subsystem> &item, pqxx::work &tx) const
{
    // TODO
}

void PGSubsystemModel::deplace_outbound_object(std::size_t id, pqxx::work &tx) const
{
    // TODO
}

}
