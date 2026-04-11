/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the project-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#include "Project.hpp"
#include "../Logging.hpp"

namespace optifol
{

Project::Project(std::string &&name) :
    Glib::ObjectBase("Project")
{
    property_name().set_value(std::move(name));
}

Project::Project(std::string &&name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Project"),
    StorageObjectBase(cobject, builder)
{
    property_name().set_value(std::move(name));
}

bool Project::operator==(const Project &other) const noexcept
{
    return std::hash<Project>{}(*this) == std::hash<Project>{}(other);
}

std::string Project::get_path() const
{
    return '/' + property_name().get_value();
}

} // namespace optifol
