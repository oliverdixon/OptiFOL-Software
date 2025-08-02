/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the DiscoveryTestExecutable storage object
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#include "DiscoveryTestExecutable.hpp"

namespace optifol
{

DiscoveryTestExecutable::DiscoveryTestExecutable(const Glib::ustring &name) :
    Glib::ObjectBase("DiscoveryTestExecutable")
{
    property_name().set_value(name);
}

DiscoveryTestExecutable::DiscoveryTestExecutable(
        const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("DiscoveryTestExecutable"),
    StorageObjectBase(cobject, builder)
{
    property_name().set_value(name);
}

Glib::RefPtr<Gio::ListStore<DiscoveryTestFixture>> DiscoveryTestExecutable::get_fixture_model() const noexcept
{
    return fixtures;
}

Glib::RefPtr<DiscoveryTestFixture> DiscoveryTestExecutable::get_default_fixture() const
{
    return fixtures->get_item(0);
}

void DiscoveryTestExecutable::add_fixture(const Glib::RefPtr<DiscoveryTestFixture> &fixture) const
{
    fixtures->append(fixture);
}

bool DiscoveryTestExecutable::operator==(const DiscoveryTestExecutable &other) const
{
    return property_name().get_value() == other.property_name().get_value();
}

bool DiscoveryTestExecutable::operator==(const Glib::ustring &other_name) const
{
    return property_name().get_value() == other_name;
}

} // namespace optifol
