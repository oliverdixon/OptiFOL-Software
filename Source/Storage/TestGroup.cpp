/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the TestGroup grouping GLib object
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#include "TestGroup.hpp"

namespace optifol
{

TestGroup::TestGroup(const Glib::ustring& name) :
    Glib::ObjectBase("TestGroup")
{
    property_name().set_value(name);
}

TestGroup::TestGroup(const Glib::ustring& name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("TestGroup"),
    StorageObjectBase(cobject, builder)
{
    property_name().set_value(name);
}

} // namespace optifol
