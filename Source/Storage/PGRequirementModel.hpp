/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed requirement storage container
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#ifndef PGREQUIREMENTMODEL_HPP
#define PGREQUIREMENTMODEL_HPP

#include "PGStorableObjectModel.hpp"
#include "Requirement.hpp"
#include "Subsystem.hpp"

namespace optifol
{

class PGRequirementModel :
        public PGStorableObjectModel<Requirement>
{
public:
    explicit PGRequirementModel(pqxx::connection& connection);

    PGRequirementModel(pqxx::connection& connection, const Subsystem& initial_subsystem,
        std::size_t initial_cache_limit = 4096);

    ~PGRequirementModel() override = default;

    void load_for_subsystem(const Subsystem& subsystem, std::size_t limit = 4096);

private:
    pqxx::result filter_objects(const std::ostringstream &sql_parameter, std::size_t maximum_return_count) const
        override;

    void emplace_object(const pqxx::row& row) override;

    void deplace_object(std::size_t id) override;
};

}

#endif
