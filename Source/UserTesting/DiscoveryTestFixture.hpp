/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/30/25.
//

#ifndef DISCOVERYTESTFIXTURE_HPP
#define DISCOVERYTESTFIXTURE_HPP

#include <gtkmm/stringlist.h>
#include "../Storage/StorageObjectBase.hpp"

namespace optifol
{

class DiscoveryTestFixture : public StorageObjectBase
{
public:
    explicit DiscoveryTestFixture(const Glib::ustring& name);

    DiscoveryTestFixture(const Glib::ustring& name, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder);

    void add_test(const Glib::ustring& name) const;

    Glib::RefPtr<Gtk::StringList> get_test_model() noexcept;

private:
    Glib::RefPtr<Gtk::StringList> test_names = Gtk::StringList::create();
};

} // namespace optifol

#endif // DISCOVERYTESTFIXTURE_HPP
