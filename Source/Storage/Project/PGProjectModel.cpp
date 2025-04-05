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
        PGProjectModel::emplace_object(row);

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

void PGProjectModel::remove_object(const Project &project)
{
    const auto it = model_contents.find(project);
    if (it != model_contents.cend())
        model_contents.erase(it);
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

void PGProjectModel::emplace_object(const pqxx::row &row)
{
    auto project = Glib::make_refptr_for_instance(new Project(
        row[0].as<std::size_t>(),
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>()
    ));

    const auto project_insertion_ref = project;

    register_object(std::move(project));
    inform_insertion(project_insertion_ref);
}

void PGProjectModel::deplace_object(const std::size_t id)
{
    remove_object(id);
}

}
