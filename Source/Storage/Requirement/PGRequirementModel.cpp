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

#include <pqxx/pqxx>

#include "PGRequirementModel.hpp"
#include "../PGChronoType.hpp"

namespace optifol
{

PGRequirementModel::PGRequirementModel(pqxx::connection &connection) :
    PGStorableObjectModelBase(connection)
{
}

std::size_t PGRequirementModel::get_item_count() const noexcept
{
    return model_contents.size();
}

void PGRequirementModel::register_requirement(Glib::RefPtr<Requirement> &&requirement)
{
    model_contents.insert(std::move(requirement));
}

Glib::RefPtr<Requirement> PGRequirementModel::get_requirement(const Requirement &requirement)
{
    const auto it = model_contents.find(requirement);
    if (it == model_contents.cend())
        return {};

    return *it;
}

Glib::RefPtr<Requirement> PGRequirementModel::get_requirement(const std::size_t requirement_id)
{
    const auto it = model_contents.find(requirement_id);
    if (it == model_contents.cend())
        return {};

    return *it;
}

void PGRequirementModel::remove_requirement(const Requirement &requirement)
{
    const auto it = model_contents.find(requirement);
    if (it != model_contents.cend())
        model_contents.erase(it);
}

void PGRequirementModel::remove_requirement(const std::size_t requirement_id)
{
    const auto it = model_contents.find(requirement_id);
    if (it != model_contents.cend())
        model_contents.erase(it);
}

pqxx::result PGRequirementModel::filter_objects(const std::ostringstream &sql_parameter,
                                                const std::size_t maximum_return_count) const
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT requirement.id, name, created_at, last_modified, sentence, priority, description, test_id "
        "FROM requirement JOIN UNNEST($1::bigint[]) AS filter(id) ON requirement.id = filter.id LIMIT $2;",
        pqxx::params{ sql_parameter.str(), maximum_return_count }
    )};

    tx.commit();
    return result;
}

void PGRequirementModel::emplace_object(const pqxx::row &row)
{
    // TODO: need a better way of matching fields to indexes

    std::optional<std::size_t> test_id;
    if (!row[7].is_null())
        test_id.emplace(row[7].as<std::size_t>());

    register_requirement(Glib::make_refptr_for_instance(new Requirement(
        row[0].as<std::size_t>(),
        row[1].as<std::string>(),
        row[2].as<std::chrono::system_clock::time_point>(),
        row[3].as<std::chrono::system_clock::time_point>(),
        row[4].as<std::string>(),
        row[5].as<std::size_t>(),
        row[6].as<std::string>(),
        test_id,
        0 // TODO: stakeholder
    )));
}

void PGRequirementModel::deplace_object(const std::size_t id)
{
    remove_requirement(id);
}

}
