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

void PGRequirementModel::register_object(Glib::RefPtr<Requirement> &&requirement)
{
    model_contents.insert(std::move(requirement));
}

Glib::RefPtr<Requirement> PGRequirementModel::get_object(const Requirement &requirement)
{
    const auto it = model_contents.find(requirement);
    if (it == model_contents.cend())
        return {};

    return *it;
}

Glib::RefPtr<Requirement> PGRequirementModel::get_object(const std::size_t requirement_id)
{
    const auto it = model_contents.find(requirement_id);
    if (it == model_contents.cend())
        return {};

    return *it;
}

void PGRequirementModel::remove_object(const std::size_t requirement_id)
{
    const auto it = model_contents.find(requirement_id);
    if (it != model_contents.cend())
        model_contents.erase(it);
}

void PGRequirementModel::load_for_subsystem(Glib::RefPtr<Subsystem> &&subsystem)
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
            "SELECT requirement.id, subsystem_id, name, created_at, last_modified, sentence, priority, description, "
            "test_id FROM requirement WHERE subsystem_id = $1", pqxx::params{ subsystem->get_controller_id() }
    )};

    tx.commit();

    for (auto &&row: result)
        enqueue_inbound_load(std::move(row));

    flush_inbound_insert();
}

pqxx::result PGRequirementModel::filter_objects(const std::ostringstream &sql_parameter,
                                                const std::size_t maximum_return_count) const
{
    pqxx::work tx{connection};
    const pqxx::result result{tx.exec(
        "SELECT requirement.id, subsystem_id, name, created_at, last_modified, sentence, priority, description, "
        "test_id FROM requirement JOIN UNNEST($1::bigint[]) AS filter(id) ON requirement.id = filter.id LIMIT $2;",
        pqxx::params{ sql_parameter.str(), maximum_return_count }
    )};

    tx.commit();
    return result;
}

void PGRequirementModel::emplace_inbound_object(const pqxx::row &row)
{
    std::optional<std::size_t> test_id;
    if (!row[DBFieldIdx::TestID].is_null())
        test_id.emplace(row[8].as<std::size_t>());

    auto requirement = Glib::make_refptr_for_instance(new Requirement(
        row[DBFieldIdx::ID].as<std::size_t>(),
        row[DBFieldIdx::SubsystemID].as<std::size_t>(),
        row[DBFieldIdx::Name].as<std::string>(),
        row[DBFieldIdx::CreatedAt].as<std::chrono::system_clock::time_point>(),
        row[DBFieldIdx::LastModified].as<std::chrono::system_clock::time_point>(),
        row[DBFieldIdx::Sentence].as<std::string>(),
        row[DBFieldIdx::Priority].as<std::size_t>(),
        row[DBFieldIdx::Description].as<std::string>(),
        test_id,
        0 // TODO: stakeholder
    ));

    auto requirement_insertion_ref = requirement;
    register_object(std::move(requirement));
    inform_insertion(std::move(requirement_insertion_ref));
}

void PGRequirementModel::update_inbound_object(const pqxx::row &row)
{
    // TODO
}

void PGRequirementModel::deplace_inbound_object(const std::size_t id)
{
    remove_object(id);
    inform_deletion(id);
}

void PGRequirementModel::emplace_outbound_object(const Glib::RefPtr<Requirement> &item, pqxx::work &tx) const
{
    tx.exec("INSERT INTO requirement (id, description, subsystem_id, name, created_at, last_modified, sentence, "
            "test_id, priority) VALUES ($1, $2, $3, $4, $5);",
            pqxx::params{ item->get_controller_id(), item->get_description(), item->get_relevant_subsystem_tag(),
                item->get_identifier(), item->get_creation_time(), item->get_modified_time(), item->get_statement(),
                item->get_test(), item->get_priority() });
}

void PGRequirementModel::update_outbound_object(const Glib::RefPtr<Requirement> &item, pqxx::work &tx) const
{
    // TODO
}

void PGRequirementModel::deplace_outbound_object(std::size_t id, pqxx::work &tx) const
{
    // TODO
}

}
