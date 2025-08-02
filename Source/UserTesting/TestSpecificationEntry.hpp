/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the TestSpecificationEntry storage object
 * @author Oliver Dixon
 * @date 2025-07-31
 * @version Development
 */

#ifndef TESTSPECIFICATIONENTRY_HPP
#define TESTSPECIFICATIONENTRY_HPP

#include "../Storage/StorageObjectBase.hpp"
#include "DiscoveryTestExecutable.hpp"

namespace optifol
{

/**
 * @class TestSpecificationEntry
 * @brief Provides a GObject container for aggregation of discovered tests with a DiscoveryTestExecutable,
 *  DiscoveryTestFixture, and Glib::ustring test name. Discovered tests are sole metadata objects containing printable
 *  data returned from test discovery.
 * @see Test for the runnable non-discovery counterpart.
 */
class TestSpecificationEntry : public StorageObjectBase
{
public:
    /**
     * @brief Construct a new TestSpecificationEntry to register as a GType with empty properties.
     */
    TestSpecificationEntry();

    /**
     * @brief Construct a new TestSpecificationEntry to register as a GType with empty properties and an existing C
     *  object.
     * @param cobject Existing C object
     * @param builder Existing source Gtk::Builder
     */
    TestSpecificationEntry(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    /**
     * @brief Retrieves a read-write proxy for the DiscoveryTestExecutable executable property.
     * @return A read-write proxy for the executable.
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::RefPtr<DiscoveryTestExecutable>> property_executable();

    /**
     * @brief Retrieves a read-write proxy for the DiscoveryTestFixture fixture property.
     * @return A read-write proxy for the fixture.
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::RefPtr<DiscoveryTestFixture>> property_fixture();

    /**
     * @brief Retrieves a read-only proxy for the DiscoveryTestExecutable executable property.
     * @return A read-only proxy for the executable.
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestExecutable>> property_executable() const;

    /**
     * @brief Retrieves a read-only proxy for the DiscoveryTestFixture fixture property.
     * @return A read-only proxy for the fixture.
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestFixture>> property_fixture() const;

private:
    /**
     * @brief Establish callbacks such that the internal intra-model state stays correctly synchronised.
     */
    void setup_sync_callbacks();

    Glib::Property<Glib::RefPtr<DiscoveryTestExecutable>> executable;

    Glib::Property<Glib::RefPtr<DiscoveryTestFixture>> fixture;
};

} // namespace optifol

#endif // TESTSPECIFICATIONENTRY_HPP
