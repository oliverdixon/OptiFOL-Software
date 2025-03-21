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
#include "PGChronoType.hpp"
#include "../Logging.hpp"

namespace optifol
{

PGProjectModel::PGProjectModel(pqxx::connection &connection, std::size_t initial_cache_limit):
    PGStorableObjectModel(connection)
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec("SELECT id, name, created_at, last_modified FROM project ORDER BY last_modified "
                                      "DESC LIMIT $1;", pqxx::params{initial_cache_limit})};
    tx.commit();

    for (const auto& row : result)
        PGProjectModel::emplace_object(row);

    assert(get_n_items() <= initial_cache_limit);
}

Glib::RefPtr<PGSubsystemModel> PGProjectModel::get_subsystem_model(const Glib::RefPtr<Project> &project) const
{
    const auto& ss_model_it = subsystem_models.find(project);
    return ss_model_it == subsystem_models.cend() ? nullptr : ss_model_it->second;
}

Glib::RefPtr<PGSubsystemModel> PGProjectModel::get_subsystem_model(const std::size_t project_id) const
{
    const auto& ss_model_it = subsystem_models.find(project_id);
    return ss_model_it == subsystem_models.cend() ? nullptr : ss_model_it->second;
}

void PGProjectModel::load()
{
    PGStorableObjectModelBase::pq_load();

    // TODO: can we do any sanity assert-checks here?
    for (const auto& ss_model : subsystem_models)
        ss_model.second->load();
}

void PGProjectModel::unload()
{
    PGStorableObjectModelBase::pq_unload();

    /*
     * We need to propagate the unload queue for all subsystems owned by projects encapsulated by this project model.
     * The above PGStorableObjectModelBase::pq_unload() call should erase key-value pairs from the subsystem model map
     * as they are deleted from the project model via PGProjectModel::deplace_object(std::size_t). The following
     * assertion is a quick, dumb check to ensure that the map keys are probably in sync with the project model
     * contents.
     */
    assert(get_n_items() == subsystem_models.size());

    for (const auto& ss_model : subsystem_models)
        ss_model.second->unload();
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
    const auto id = row[0].as<std::size_t>();
    LOG4CXX_INFO(Logging::get_logger(), "Loading project with ID " << std::to_string(id));

    auto loaded = Glib::make_refptr_for_instance(new Project(
        id,
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>())
    );

    /*
     * Append the loaded project to ourselves (since we are a Gio::ListModel), and provide shared ownership with the
     * subsystem such that a slot for a subsystem model is associated with the master project.
     */
    append(loaded);
    subsystem_models.emplace(loaded, Glib::make_refptr_for_instance(new PGSubsystemModel(connection, *loaded)));
}

void PGProjectModel::deplace_object(std::size_t id)
{
    auto [found, position] = find(dummy_base,
        [id](const Glib::RefPtr<const Project>& candidate, const Glib::RefPtr<const Project>& dummy) -> auto
        {
            std::ignore = dummy;
            return candidate->get_controller_id() == id;
        }
    );

    if (found) {
        LOG4CXX_INFO(Logging::get_logger(), "Unloading project with ID " << std::to_string(id));

        /*
         * Cumbersome, but std::unordered_map::erase doesn't support transparent lookup. (Why not?) Anyway, remove the
         * subsystem slot thus calling the destructing of the mapped value (subsystem model), then remove the project
         * itself from ourselves as the project model.
         */
        const auto project_it = subsystem_models.find(id);
        subsystem_models.erase(project_it);
        remove(position);
    }
}

}
