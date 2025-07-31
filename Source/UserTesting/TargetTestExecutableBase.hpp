/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/29/25.
//

#ifndef TARGETTESTEXECUTABLE_HPP
#define TARGETTESTEXECUTABLE_HPP

#include <giomm/filemonitor.h>
#include <gtkmm/stringlist.h>
#include <unordered_set>

#include "../Storage/StorageObjectBase.hpp"
#include "TestResult.hpp"

namespace optifol
{

class Requirement;

class TargetTestExecutableBase : public StorageObjectBase
{
public:
    TargetTestExecutableBase() = default;

    TargetTestExecutableBase(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    void receive_result(std::unique_ptr<TestResult>&& test_result);

    void distribute_result(Requirement &) const;

    Glib::RefPtr<Gtk::StringList> get_fixtures_model() const;

protected:
    // TODO should be a model of TestFixtures (new class)
    Glib::RefPtr<Gtk::StringList> fixtures_model = Gtk::StringList::create();

private:
    Glib::RefPtr<Gio::FileMonitor> file_monitor;
    std::unordered_set<std::shared_ptr<TestResult>> received_blob;
    bool is_outdated = false;
};

} // namespace optifol

#endif // TARGETTESTEXECUTABLE_HPP
