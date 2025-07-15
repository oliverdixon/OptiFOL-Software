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

Test::Test() :
    Glib::ObjectBase("Test"),
    target_executable(*this, "Test-target-executable"),
    test_suite(*this, "Test-test-suite"),
    test_name(*this, "Test-test-name"),
    execution_time_ms(*this, "Test-execution-time-ms")
{
}

Test::Test(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Test"),
    StorageObjectBase(cobject, builder),
    target_executable(*this, "Test-target-executable"),
    test_suite(*this, "Test-test-suite"),
    test_name(*this, "Test-test-name"),
    execution_time_ms(*this, "Test-execution-time-ms")
{
}

Glib::PropertyProxy<Glib::ustring> Test::property_target_executable()
{
    return target_executable.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Test::property_test_suite()
{
    return test_suite.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Test::property_test_name()
{
    return test_name.get_proxy();
}

Glib::PropertyProxy<std::size_t> Test::property_execution_time_ms()
{
    return execution_time_ms.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_target_executable() const
{
    return target_executable.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_test_suite() const
{
    return test_suite.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_test_name() const
{
    return test_name.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::size_t> Test::property_execution_time_ms() const
{
    return execution_time_ms.get_proxy();
}

} // namespace optifol
