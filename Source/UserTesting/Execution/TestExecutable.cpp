/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the abstract TargetTestExecutableBase
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#include "TestExecutable.hpp"

namespace optifol
{

TestExecutable::TestExecutable(const Glib::ustring &executable_path) :
    Glib::ObjectBase("TestExecutable")
{
    property_name().set_value(executable_path);
}

TestExecutable::TestExecutable(const Glib::ustring &executable_path, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("TestExecutable"),
    StorageObjectBase(cobject, builder)
{
    property_name().set_value(executable_path);
}

bool TestExecutable::operator==(const TestExecutable &other) const noexcept
{
    return property_name().get_value() == other.property_name().get_value();
}

} // namespace optifol
