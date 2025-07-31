/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/30/25.
//

#ifndef DISCOVERYTESTEXECUTABLE_HPP
#define DISCOVERYTESTEXECUTABLE_HPP

#include <giomm/liststore.h>
#include "../../Storage/StorageObjectBase.hpp"
#include "DiscoveryTestFixture.hpp"

namespace optifol
{

class DiscoveryTestExecutable : public StorageObjectBase
{
public:
    Glib::RefPtr<Gio::ListStore<DiscoveryTestFixture>> get_fixture_model() const noexcept;

    Glib::RefPtr<DiscoveryTestFixture> get_default_fixture() const;

    bool operator==(const DiscoveryTestExecutable & other) const;

    bool operator==(const Glib::ustring& other_name) const;

protected:
    explicit DiscoveryTestExecutable(const Glib::ustring& name);

    DiscoveryTestExecutable(const Glib::ustring& name, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder);

    void add_fixture(const Glib::RefPtr<DiscoveryTestFixture>& fixture) const;

private:
    Glib::RefPtr<Gio::ListStore<DiscoveryTestFixture>> fixtures = Gio::ListStore<DiscoveryTestFixture>::create();
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
