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

#include <pqxx/pqxx>

#include "PGProjectModel.hpp"
#include "../PGChronoType.hpp"

namespace optifol
{

PGProjectModel::PGProjectModel(pqxx::connection &connection, const std::size_t initial_cache_limit) :
    PGStorableObjectModelBase(connection)
{
    pqxx::work tx{connection};
    const pqxx::result result{
        tx.exec("SELECT id, name, created_at, last_modified FROM project ORDER BY last_modified "
                "DESC LIMIT $1;", pqxx::params{initial_cache_limit})
    };
    tx.commit();

    for (const auto &row: result)
        PGProjectModel::emplace_inbound_object(row);

    assert(PGProjectModel::get_item_count() <= initial_cache_limit);
}

std::size_t PGProjectModel::get_item_count() const noexcept
{
    return model_contents.size();
}

void PGProjectModel::register_object(Glib::RefPtr<Project> &&project)
{
    model_contents.insert(std::move(project));
}

Glib::RefPtr<Project> PGProjectModel::get_object(const Project &project)
{
    const auto it = model_contents.find(project);
    if (it == model_contents.cend())
        return {};

    return *it;
}

Glib::RefPtr<Project> PGProjectModel::get_object(const std::size_t project_id)
{
    const auto it = model_contents.find(project_id);
    if (it == model_contents.cend())
        return {};

    return *it;
}

void PGProjectModel::remove_object(const std::size_t project_id)
{
    const auto it = model_contents.find(project_id);
    if (it != model_contents.cend())
        model_contents.erase(it);
}

pqxx::result PGProjectModel::filter_objects(const std::ostringstream& sql_parameter,
                                            const std::size_t maximum_return_count) const
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT * FROM project JOIN UNNEST($1::bigint[]) AS filter(id) ON project.id = filter.id LIMIT $2;",
        pqxx::params{ sql_parameter.str(), maximum_return_count }
    )};

    tx.commit();
    return result;
}

void PGProjectModel::emplace_inbound_object(const pqxx::row &row)
{
    auto project = Glib::make_refptr_for_instance(new Project(
        row[DBFieldIdx::ID].as<std::size_t>(),
        row[DBFieldIdx::Name].as<std::string>(),
        row[DBFieldIdx::CreatedAt].as<std::chrono::system_clock::time_point>(),
        row[DBFieldIdx::LastModified].as<std::chrono::system_clock::time_point>()
    ));

    auto project_insertion_ref = project;
    register_object(std::move(project));
    inform_insertion(std::move(project_insertion_ref));
}

void PGProjectModel::update_inbound_object(const pqxx::row &row)
{
    const auto project_idx = row[DBFieldIdx::ID].as<std::size_t>();
    const auto project_it = model_contents.find(project_idx);

    if (project_it == model_contents.end())
        emplace_inbound_object(row);
    else {
        const auto project = *project_it;

        const std::string_view name = row[DBFieldIdx::Name].as<std::string_view>();
        if (project->get_identifier() != name) project->set_identifier(std::string(name));

        // TODO: all other fields.
    }
}

void PGProjectModel::deplace_inbound_object(const std::size_t id)
{
    remove_object(id);
    inform_deletion(id);
}

void PGProjectModel::emplace_outbound_object(const Glib::RefPtr<Project> &item, pqxx::work& tx) const
{
    tx.exec("INSERT INTO project (id, name, created_at, last_modified) VALUES ($1, $2, $3, $4);",
        pqxx::params{ item->get_controller_id(), item->get_identifier(), item->get_creation_time(),
            item->get_modified_time() });
}

void PGProjectModel::update_outbound_object(const Glib::RefPtr<Project> &item, pqxx::work &tx) const
{
    // TODO
}

void PGProjectModel::deplace_outbound_object(const std::size_t id, pqxx::work &tx) const
{
    // TODO
}

}
