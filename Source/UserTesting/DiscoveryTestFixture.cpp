/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/30/25.
//

#include "DiscoveryTestFixture.hpp"

namespace optifol
{

DiscoveryTestFixture::DiscoveryTestFixture(const Glib::ustring &name) :
    Glib::ObjectBase("DiscoveryTestFixture")
{
    property_name().set_value(name);
}

DiscoveryTestFixture::DiscoveryTestFixture(
        const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("DiscoveryTestFixture"),
    StorageObjectBase(cobject, builder)
{
    property_name().set_value(name);
}

void DiscoveryTestFixture::add_test(const Glib::ustring &name) const
{
    test_names->append(name);
}

Glib::RefPtr<Gtk::StringList> DiscoveryTestFixture::get_test_model() noexcept
{
    return test_names;
}

} // namespace optifol
