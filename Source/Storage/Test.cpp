/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/15/25.
//

#include "Test.hpp"

namespace optifol
{

Test::Test(const std::string& target_executable) :
    Glib::ObjectBase("Test"),
    target_executable(*this, "Test-target-executable", target_executable)
{
}

Test::Test(const std::string &target_executable,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Test"),
    StorageObjectBase(cobject, builder),
    target_executable(*this, "Test-target-executable", target_executable)
{
}

Glib::PropertyProxy<Glib::ustring> Test::property_target_executable()
{
    return target_executable.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_target_executable() const
{
    return target_executable.get_proxy();
}

} // namespace optifol
