/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the DiscoveryTestExecutable storage object
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef DISCOVERYTESTEXECUTABLE_HPP
#define DISCOVERYTESTEXECUTABLE_HPP

#include <giomm/liststore.h>

#include "../../Storage/StorageObjectBase.hpp"
#include "DiscoveryTestFixture.hpp"

namespace optifol
{

/**
 * @class DiscoveryTestExecutable
 * @brief Describes a testing framework-agnostic executable used to discover and model software unit fixtures and tests
 *  thereof. Derived classes populate the executable with fixtures during or after framework-dependent discovery, and
 *  consumers query the model from the base class.
 */
class DiscoveryTestExecutable : public StorageObjectBase
{
public:
    /**
     * @brief Retrieves the fixtures model to use in a Gtk::DropDown or Gtk::ListView.
     * @return A ref-counted pointer to the fixtures model, containing a single entry for each fixture within the
     *  discovered executable.
     */
    Glib::RefPtr<Gio::ListStore<DiscoveryTestFixture>> get_fixture_model() const noexcept;

    /**
     * @brief Retrieves the first fixture object available within the discovered executable.
     * @return A ref-counted pointer to the first entry in the fixtures model.
     */
    Glib::RefPtr<DiscoveryTestFixture> get_default_fixture() const;

    /**
     * @brief Compare the test executable with an object of the same type.
     * @param other The other test executable.
     * @return Do the test executable objects refer to the same executable? Equality is determined by path.
     */
    bool operator==(const DiscoveryTestExecutable & other) const;

    /**
     * @brief Compare the test executable with the given exectable path.
     * @param other_executable_path The path of another executable on the file-system.
     * @return Does the test executable object and executable at the given path refer to the same executable?
     */
    bool operator==(const Glib::ustring& other_executable_path) const;

protected:
    /**
     * @brief Construct a new framework-agnostic test executable. Note that for the base constructor, no discovery is
     *  performed and an empty fixture model is instantiated.
     * @param executable_path The path of the executable to be subject to discovery.
     */
    explicit DiscoveryTestExecutable(const Glib::ustring& executable_path);

    /**
     * @brief Construct a new framework-agnostic test executable. Note that for the base constructor, no discovery is
     *  performed and an empty fixture model is instantiated.
     * @param executable_path The path of the executable to be subject to discovery.
     * @param cobject Glib C object.
     * @param builder Existing Gtk::Builder instance.
     */
    DiscoveryTestExecutable(const Glib::ustring& executable_path, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder);

    /**
     * @brief Share a new fixture with the model. The shared fixture is appended to the internal model.
     * @param fixture The fixture to share.
     */
    void add_fixture(Glib::RefPtr<DiscoveryTestFixture> fixture) const;

private:
    const Glib::RefPtr<Gio::ListStore<DiscoveryTestFixture>> fixtures;
};

} // namespace optifol

template<>
struct std::hash<optifol::DiscoveryTestExecutable>
{
    using is_transparent = void;

    std::size_t operator()(const optifol::DiscoveryTestExecutable * object) const noexcept
    {
        return object->hash();
    }

    std::size_t operator()(const Glib::ustring& name) const noexcept
    {
        return std::hash<std::string>{}(name);
    }

    std::size_t operator()(const optifol::DiscoveryTestExecutable & object) const noexcept
    {
        return object.hash();
    }

    std::size_t operator()(const std::shared_ptr<optifol::DiscoveryTestExecutable> &shared_hashable) const noexcept
    {
        return shared_hashable->hash();
    }
};

#endif // DISCOVERYTESTEXECUTABLE_HPP
