/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/31/25.
//

#include "TestSpecificationEntry.hpp"

namespace optifol
{

TestSpecificationEntry::TestSpecificationEntry() :
    Glib::ObjectBase("TestSpecificationEntry"),
    executable(*this, "TestSpecificationEntry-executable"),
    fixture(*this, "TestSpecificationEntry-fixture"),
    test(*this, "TestSpecificationEntry-test")
{
    setup_sync_callbacks();
}

TestSpecificationEntry::TestSpecificationEntry(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("TestSpecificationEntry"),
    StorageObjectBase(cobject, builder),
    executable(*this, "TestSpecificationEntry-executable"),
    fixture(*this, "TestSpecificationEntry-fixture"),
    test(*this, "TestSpecificationEntry-test")
{
    setup_sync_callbacks();
}

Glib::PropertyProxy<Glib::RefPtr<DiscoveryTestExecutable>> TestSpecificationEntry::property_executable()
{
    return executable.get_proxy();
}

Glib::PropertyProxy<Glib::RefPtr<DiscoveryTestFixture>> TestSpecificationEntry::property_fixture()
{
    return fixture.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> TestSpecificationEntry::property_test()
{
    return test.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestExecutable>> TestSpecificationEntry::property_executable() const
{
    return executable.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestFixture>> TestSpecificationEntry::property_fixture() const
{
    return fixture.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> TestSpecificationEntry::property_test() const
{
    return test.get_proxy();
}

void TestSpecificationEntry::setup_sync_callbacks()
{
    property_executable().signal_changed().connect([this]
    {
        property_fixture().set_value(executable.get_value()->get_default_fixture());
    });
}

} // namespace optifol
