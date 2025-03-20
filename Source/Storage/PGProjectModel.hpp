/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed project storage container
 * @author Oliver Dixon
 * @date 2025-02-28
 * @version Development
 */

#ifndef PGPROJECTMODEL_HPP
#define PGPROJECTMODEL_HPP

#include "PGStorableObjectModel.hpp"
#include "PGSubsystemModel.hpp"
#include "Project.hpp"

namespace optifol
{

/**
 * @class PGProjectModel
 * @brief The PGStorableObjectModel specialised for project-level stored objects
 */
class PGProjectModel :
        public PGStorableObjectModel<Project>
{
public:
    /**
     * @brief Construct a project cache container and cache a set of most recently modified projects
     * @param connection The established PostgreSQL database connection
     * @param initial_cache_limit The maximum number of projects to initially load into the cache
     * @post The number of cached projects does not exceed the defined limit
     */
    explicit PGProjectModel(pqxx::connection& connection, std::size_t initial_cache_limit = 128);

    Glib::RefPtr<PGSubsystemModel> get_subsystem_model(const Glib::RefPtr<Project>& project) const;

private:
    pqxx::result filter_objects(const std::ostringstream& sql_parameter) const override;

    void emplace_object(const pqxx::row& row) override;

    std::unordered_map<Glib::RefPtr<Project>, Glib::RefPtr<PGSubsystemModel>> subsystem_models;

    const Glib::RefPtr<const Project> dummy_base = Glib::make_refptr_for_instance(new Project(0, {}, {}, {}));
};

}

#endif
