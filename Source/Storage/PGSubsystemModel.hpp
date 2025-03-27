/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed subsystem storage container
 * @author Oliver Dixon
 * @date 2025-03-18
 * @version Development
 */

#ifndef PGSUBSYSTEMMODEL_HPP
#define PGSUBSYSTEMMODEL_HPP

#include "PGRequirementModel.hpp"
#include "PGStorableObjectModel.hpp"
#include "Project.hpp"
#include "StorageEqualityFunctor.hpp"
#include "Subsystem.hpp"

namespace optifol
{

/**
 * @class PGSubsystemModel
 * @brief The PGStorableObjectModel specialised for subsystem-level stored objects
 */
class PGSubsystemModel :
        public PGStorableObjectModel<Subsystem>
{
public:
    /**
     * @brief Construct a subsystem cache container
     * @param connection The established PostgreSQL database connection
     * @post The number of cached subsystems is zero
     */
    explicit PGSubsystemModel(pqxx::connection& connection);

    /**
     * @brief Construct a subsystem cache container populated with the subsystems of the given project, up to the
     *  optionally defined initial cache size limit
     * @param connection The established PostgreSQL database connection
     * @param initial_project The initial project with which the subsystem model should be associated. If provided, all
     *  subsystems of the given project will be loaded into the model, up to the defined limit.
     * @param initial_cache_limit The maximum number of subsystems to initially load into the cache
     * @post The number of cached subsystems does not exceed the defined limit
     */
    PGSubsystemModel(pqxx::connection& connection, const Project& initial_project,
        std::size_t initial_cache_limit = 128);

    ~PGSubsystemModel() override;

    void load_for_project(const Project& project, std::size_t limit = 128);

private:
    pqxx::result filter_objects(const std::ostringstream &sql_parameter, std::size_t maximum_return_count) const
        override;

    void emplace_object(const pqxx::row& row) override;

    void deplace_object(std::size_t id) override;

    std::unordered_map<Glib::RefPtr<Subsystem>, Glib::RefPtr<PGRequirementModel>, StorageHashFunctor<Project>,
        StorageEqualityFunctor<Subsystem>> requirement_models;

    const Glib::RefPtr<const Subsystem> dummy_base = Glib::make_refptr_for_instance(new Subsystem(0, {}, {}, {}));
};

}

#endif
