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

#include "../../Storage/StorageObjectBase.hpp"
#include "DiscoveryTestExecutable.hpp"

namespace optifol
{

/**
 * @class TestSpecificationEntry
 * @brief Provides a GObject container for description of a test specification. A test specification can be
 * used to construct a Test and holds a test executable, a fixture, and a test name.
 * @details Test specifications are useful during the <i>discovery</i> phase of user testing. In particular,
 * the user selects a test executable provided by a unit-testing framework (e.g. Google Test), the executable
 * is queried either from the file-system or from an internal cache, and string models are constructed to
 * detain the available fixtures and tests thereof. These properties are encoded by classes
 * DiscoveryTestExecutable and DiscoveryTestFixture respectively, and held under shared ownership by the test
 * specification.
 */
class TestSpecificationEntry : public StorageObjectBase
{
public:
    /**
     * @brief Construct a new TestSpecificationEntry to register as a GType with empty properties.
     */
    TestSpecificationEntry();

    /**
     * @brief Construct a new TestSpecificationEntry to register as a GType with empty properties and an
     * existing C object.
     * @param cobject Existing C object
     * @param builder Existing source Gtk::Builder
     */
    TestSpecificationEntry(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    /**
     * @brief Copy-construct a TestSpecificationEntry from the given template entry.
     * @param template_entry The entry to clone.
     * @todo URGENT TODO doesn't work! :)
     */
    TestSpecificationEntry(const TestSpecificationEntry &template_entry);

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
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestExecutable>>
    property_executable() const;

    /**
     * @brief Retrieves a read-only proxy for the DiscoveryTestFixture fixture property.
     * @return A read-only proxy for the fixture.
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::RefPtr<DiscoveryTestFixture>> property_fixture() const;

private:
    /**
     * @brief Establish callbacks such that the internal intra-model state stays correctly synchronised.
     * @details In particular, changes in the @ref executable will unidirectionally propagate to the @ref
     * fixture.
     */
    void setup_sync_callbacks();

    /**
     * @brief The discovered executable shared-ownership reference property.
     * @details This property shares ownership for a DiscoveryTestExecutable object. Shared ownership is
     * necessary here; multiple TestSpecificationEntry objects are likely to refer to the same
     * DiscoveryTestExecutable. Due to this, it is likely that callers (c.f. ManageTestsPopover) will want to
     * maintain some sort of set-based cache and provide references. Lifetime guarantees cannot be provided on
     * the cache, so shared ownership is necessary.
     */
    Glib::Property<Glib::RefPtr<DiscoveryTestExecutable>> executable;

    /**
     * @brief The discovered fixture shared-ownership reference property.
     * @details This property shares ownership for a DiscoveryTestFixture object. Shared ownership is
     * unfortunately necessary here. The DiscoveryTestFixture will, under defined operating conditions, be a
     * member of the fixtures detained by @ref executable. Although lifetime guarantees can be placed on the
     * DiscoveryTestExecutable, Glib does not guarantee stable pointers within the internal Gio::ListModel;
     * hence we need to claim ownership locally.
     */
    Glib::Property<Glib::RefPtr<DiscoveryTestFixture>> fixture;
};

} // namespace optifol

#endif // TESTSPECIFICATIONENTRY_HPP
