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
#include "DiscoveryTestFixture.hpp"

namespace optifol
{

DiscoveryTestExecutable::DiscoveryTestExecutable(const Glib::ustring &executable_path) :
    Glib::ObjectBase("DiscoveryTestExecutable"),
    fixtures(Gio::ListStore<DiscoveryTestFixture>::create())
{
    property_name().set_value(executable_path);
}

DiscoveryTestExecutable::DiscoveryTestExecutable(const Glib::ustring &executable_path,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("DiscoveryTestExecutable"),
    StorageObjectBase(cobject, builder),
    fixtures(Gio::ListStore<DiscoveryTestFixture>::create())
{
    property_name().set_value(executable_path);
}

Glib::RefPtr<Gio::ListStore<DiscoveryTestFixture>> DiscoveryTestExecutable::get_fixture_model() const noexcept
{
    return fixtures;
}

Glib::RefPtr<DiscoveryTestFixture> DiscoveryTestExecutable::get_default_fixture() const
{
    return fixtures->get_item(0);
}

void DiscoveryTestExecutable::add_fixture(Glib::RefPtr<DiscoveryTestFixture> fixture) const
{
    fixtures->append(std::move(fixture));
}

bool DiscoveryTestExecutable::operator==(const DiscoveryTestExecutable &other) const
{
    return property_name().get_value() == other.property_name().get_value();
}

bool DiscoveryTestExecutable::operator==(const Glib::ustring &other_executable_path) const
{
    return property_name().get_value() == other_executable_path;
}

} // namespace optifol
