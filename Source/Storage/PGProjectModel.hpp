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

#include "PGStorableObjectModelBase.hpp"
#include "Project.hpp"
#include "ProjectModel.hpp"

namespace optifol
{

/**
 * @class PGProjectModel
 * @brief TODO
 */
class PGProjectModel :
        virtual public ProjectModel,
        virtual public PGStorableObjectModelBase<Project>
{
public:
    /**
     * @brief Construct a project cache container and cache a set of most recently modified projects
     * @param connection The established PostgreSQL database connection
     * @param initial_cache_limit The maximum number of projects to initially load into the cache
     * @post The number of cached projects does not exceed the defined limit
     */
    explicit PGProjectModel(pqxx::connection& connection, std::size_t initial_cache_limit = 128);

    void flush_inbound_insert() override;

    void flush_inbound_delete() override;

private:
    pqxx::result filter_objects(const std::ostringstream& sql_parameter, std::size_t maximum_return_count) const
        override;

    void emplace_object(const pqxx::row& row) override;

    void deplace_object(std::size_t id) override;

    const Glib::RefPtr<const Project> dummy_base = Glib::make_refptr_for_instance(new Project(0, {}, {}, {}));
};

}

#endif
