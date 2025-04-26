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

namespace optifol
{

Requirement::Requirement(std::string&& name, std::string&& statement, std::string&& description, const guint priority) :
    Glib::ObjectBase("Requirement"),
    statement(*this, "Requirement-statement"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
{
    property_name().set_value(std::move(name));
    property_statement().set_value(std::move(statement));
    property_description().set_value(std::move(description));
    property_priority().set_value(priority);
}

Requirement::Requirement(std::string&& name, std::string&& statement, std::string&& description, const guint priority,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Requirement"),
    StorageObjectBase(cobject, builder),
    statement(*this, "Requirement-statement"),
    description(*this, "Requirement-description"),
    priority(*this, "Requirement-priority")
{
    property_name().set_value(std::move(name));
    property_statement().set_value(std::move(statement));
    property_description().set_value(std::move(description));
    property_priority().set_value(priority);
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

}
