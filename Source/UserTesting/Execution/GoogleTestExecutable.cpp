/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the GoogleTestExecutable executable backend
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#include "GoogleTestExecutable.hpp"

namespace optifol
{

GoogleTestExecutable::GoogleTestExecutable(const Glib::ustring &executable_path) :
    Glib::ObjectBase("GoogleTestExecutable")
{
    property_name().set_value(executable_path);
}

GoogleTestExecutable::GoogleTestExecutable(
        const Glib::ustring &executable_path, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("GoogleTestExecutable"),
    TargetTestExecutableBase(cobject, builder)
{
    property_name().set_value(executable_path);
}

} // namespace optifol
