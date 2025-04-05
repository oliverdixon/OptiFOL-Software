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

#include "Project.hpp"
#include "../IStorableObjectModel.hpp"
#include "../PGStorableObjectModelBase.hpp"

namespace optifol
{

/**
 * @class PGProjectModel
 * @brief TODO
 */
class PGProjectModel :
        public PGStorableObjectModelBase<Project>,
        public IStorableObjectModel<Project>
{
public:
    /**
     * @brief Construct a project cache container and cache a set of most recently modified projects
     * @param connection The established PostgreSQL database connection
     * @param initial_cache_limit The maximum number of projects to initially load into the cache
     * @post The number of cached projects does not exceed the defined limit
     */
    explicit PGProjectModel(pqxx::connection& connection, std::size_t initial_cache_limit = 128);

    [[nodiscard]] std::size_t get_item_count() const noexcept override;

    void register_object(Glib::RefPtr<Project>&& project) override;

    [[nodiscard]] Glib::RefPtr<Project> get_object(const Project& project) override;

    [[nodiscard]] Glib::RefPtr<Project> get_object(std::size_t project_id) override;

    void remove_object(const Project& project) override;

    void remove_object(std::size_t project_id) override;

private:
    [[nodiscard]] pqxx::result filter_objects(const std::ostringstream& sql_parameter, std::size_t maximum_return_count)
        const override;

    void emplace_object(const pqxx::row& row) override;

    void deplace_object(std::size_t id) override;
};

}

#endif
