/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/31/25.
//

#ifndef TESTSPECIFICATIONENTRY_HPP
#define TESTSPECIFICATIONENTRY_HPP

#include "../../Storage/StorageObjectBase.hpp"
#include "../../UserTesting/IR/DiscoveryTestExecutable.hpp"

namespace optifol
{

class TestSpecificationEntry : public StorageObjectBase
{
public:
    TestSpecificationEntry();

    TestSpecificationEntry(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    [[nodiscard]] Glib::PropertyProxy<Glib::RefPtr<DiscoveryTestExecutable>> property_executable();

    [[nodiscard]] Glib::PropertyProxy<Glib::RefPtr<DiscoveryTestFixture>> property_fixture();

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_test();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestExecutable>> property_executable() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestFixture>> property_fixture() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_test() const;

private:
    void setup_sync_callbacks();

    Glib::Property<Glib::RefPtr<DiscoveryTestExecutable>> executable;

    Glib::Property<Glib::RefPtr<DiscoveryTestFixture>> fixture;

    Glib::Property<Glib::ustring> test;
};

} // namespace optifol

#endif // TESTSPECIFICATIONENTRY_HPP
