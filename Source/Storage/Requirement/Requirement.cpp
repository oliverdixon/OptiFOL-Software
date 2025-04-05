/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the requirement-level storage object
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#include "Requirement.hpp"

namespace optifol
{

Requirement::Requirement(const std::size_t id, const std::size_t relevant_subsystem_tag, std::string&& name,
        const TimeT& created_time, const TimeT& last_modified_time, std::string&& statement, const std::size_t priority,
        std::string&& description, const std::optional<std::size_t> test_id, const std::size_t stakeholder) :
    created_time(created_time),
    last_modified_time(last_modified_time),
    name(std::move(name)),
    id(id),
    statement(std::move(statement)),
    priority(priority),
    description(std::move(description)),
    test_id(test_id),
    stakeholder(stakeholder),
    relevant_subsystem_tag(relevant_subsystem_tag)
{
}

std::string Requirement::get_identifier() const
{
    return name;
}

IStorageObject::TimeT Requirement::get_creation_time() const
{
    return created_time;
}

IStorageObject::TimeT Requirement::get_modified_time() const
{
    return last_modified_time;
}

std::size_t Requirement::get_controller_id() const noexcept
{
    return id;
}

bool Requirement::operator==(const Requirement &other) const noexcept
{
    return id == other.id;
}

bool Requirement::operator==(const std::size_t other_id) const noexcept
{
    return id == other_id;
}

std::string Requirement::get_statement() const
{
    return statement;
}

std::size_t Requirement::get_priority() const
{
    return priority;
}

std::string Requirement::get_description() const
{
    return description;
}

std::optional<std::size_t> Requirement::get_test() const
{
    return test_id;
}

std::size_t Requirement::get_stakeholder() const
{
    return stakeholder;
}

void Requirement::set_identifier(const std::string &name_candidate)
{
    this->name = name_candidate;
    last_modified_time = std::chrono::system_clock::now();
}

void Requirement::set_description(const std::string &description_candidate)
{
    this->description = description_candidate;
    last_modified_time = std::chrono::system_clock::now();
}

void Requirement::attempt_set_statement(const std::string &statement_candidate)
{
    this->statement = statement_candidate;
    last_modified_time = std::chrono::system_clock::now();
}

void Requirement::attempt_set_priority(const std::string &priority_candidate)
{
    priority = std::stoul(priority_candidate);
    last_modified_time = std::chrono::system_clock::now();
}

void Requirement::attempt_set_test(const std::string &test_candidate)
{
    test_id = std::stoul(test_candidate);
    last_modified_time = std::chrono::system_clock::now();
}

void Requirement::attempt_set_stakeholder(const std::string &stakeholder_candidate)
{
    stakeholder = std::stoul(stakeholder_candidate);
    last_modified_time = std::chrono::system_clock::now();
}

std::size_t Requirement::get_relevant_subsystem_tag() const noexcept
{
    return relevant_subsystem_tag;
}

}
