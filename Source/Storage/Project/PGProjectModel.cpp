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

PGProjectModel::PGProjectModel(pqxx::connection &connection, std::size_t initial_cache_limit) :
    PGStorableObjectModelBase(connection)
{
    pqxx::work tx{connection};
    const pqxx::result result{
        tx.exec("SELECT id, name, created_at, last_modified FROM project ORDER BY last_modified "
                "DESC LIMIT $1;", pqxx::params{initial_cache_limit})
    };
    tx.commit();

    for (const auto &row: result)
        PGProjectModel::emplace_object(row);

    assert(PGProjectModel::get_item_count() <= initial_cache_limit);
}

std::size_t PGProjectModel::get_item_count() const noexcept
{
    return projects.size();
}

void PGProjectModel::register_project(Glib::RefPtr<Project> &&project)
{
    projects.insert(std::move(project));
}

Glib::RefPtr<Project> PGProjectModel::get_project(const Project &project)
{
    const auto it = projects.find(project);
    if (it == projects.cend())
        return {};

    return *it;
}

Glib::RefPtr<Project> PGProjectModel::get_project(const std::size_t project_id)
{
    const auto it = projects.find(project_id);
    if (it == projects.cend())
        return {};

    return *it;
}

void PGProjectModel::remove_project(const Project &project)
{
    const auto it = projects.find(project);
    if (it != projects.cend())
        projects.erase(it);
}

void PGProjectModel::remove_project(const std::size_t project_id)
{
    const auto it = projects.find(project_id);
    if (it != projects.cend())
        projects.erase(it);
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

void PGProjectModel::emplace_object(const pqxx::row &row)
{
    register_project(Glib::make_refptr_for_instance(new Project(
        row[0].as<std::size_t>(),
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>()
    )));
}

void PGProjectModel::deplace_object(const std::size_t id)
{
    remove_project(id);
}

}
