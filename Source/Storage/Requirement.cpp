/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the requirement-level storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#include "Requirement.hpp"

#include "AnalysisManager.hpp"

namespace optifol
{

Requirement::Requirement(std::string&& name, std::string&& statement, std::string&& description, const guint priority) :
    Glib::ObjectBase("Requirement"),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority);
}

Requirement::Requirement(std::string&& name, std::string&& statement, std::string&& description, const guint priority,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Requirement"),
    StorageObjectBase(cobject, builder),
    statement(*this, "Requirement-statement"),
    normalised_statement(*this, "Requirement-normalised"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
{
    setup_properties(std::move(name), std::move(statement), std::move(description), priority);
}

Glib::PropertyProxy<Glib::ustring> Requirement::property_statement()
{
    return statement.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Requirement::property_description()
{
    return description.get_proxy();
}

Glib::PropertyProxy<guint> Requirement::property_priority()
{
    return priority.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Requirement::property_normalised()
{
    return normalised_statement.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Requirement::property_statement() const
{
    return statement.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Requirement::property_description() const
{
    return description.get_proxy();
}

Glib::PropertyProxy_ReadOnly<guint> Requirement::property_priority() const
{
    return priority.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Requirement::property_normalised() const
{
    return normalised_statement.get_proxy();
}

std::string Requirement::get_formatted_statement() const
{
    return formatted_input_statement;
}

void Requirement::setup_properties(std::string&& name, std::string&& statement, std::string&& description,
                                   const guint priority)
{
    property_statement().signal_changed().connect([this]
    {
        if (!property_statement().get_value().empty()) {
            auto parsed_ast = AnalysisManager::parse_sentence(property_statement().get_value());
            formatted_input_statement = AnalysisManager::get_text(parsed_ast.get());

            cnf_ast = AnalysisManager::normalise_sentence(
                std::move(parsed_ast),
                AnalysisManager::NormalisationExtent::ImplicationElimination);

            property_normalised().set_value(AnalysisManager::get_text(cnf_ast.get()));
        } else {
            cnf_ast = nullptr;
            property_normalised().set_value({});
        }
    });

    property_name().set_value(std::move(name));
    property_statement().set_value(std::move(statement));
    property_description().set_value(std::move(description));
    property_priority().set_value(priority);
}

}
