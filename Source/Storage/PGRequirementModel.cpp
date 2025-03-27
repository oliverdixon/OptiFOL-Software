/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the PostgreSQL-backed requirement storage container
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#include "PGRequirementModel.hpp"

namespace optifol
{

PGRequirementModel::PGRequirementModel(pqxx::connection &connection) :
    PGStorableObjectModel(connection)
{
}

PGRequirementModel::PGRequirementModel(pqxx::connection &connection, const Subsystem &initial_subsystem,
        const std::size_t initial_cache_limit):
    PGStorableObjectModel(connection)
{
    std::ignore = initial_subsystem;
    std::ignore = initial_cache_limit;
}

pqxx::result PGRequirementModel::filter_objects(const std::ostringstream &sql_parameter,
        const std::size_t maximum_return_count) const
{
    std::ignore = sql_parameter;
    std::ignore = maximum_return_count;

    return {};
}

void PGRequirementModel::emplace_object(const pqxx::row &row)
{
    std::ignore = row;
}

void PGRequirementModel::deplace_object(const std::size_t id)
{
    std::ignore = id;
}

}
