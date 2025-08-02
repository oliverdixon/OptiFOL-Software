/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the abstract TargetTestExecutableBase
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef TARGETTESTEXECUTABLE_HPP
#define TARGETTESTEXECUTABLE_HPP

#include <giomm/filemonitor.h>
#include <unordered_set>

#include "../../Storage/StorageObjectBase.hpp"

namespace optifol
{

class Requirement;
class DiscoveryTestExecutable;
class TestResult;

/**
 * @class TestExecutable
 * @brief Describes a framework-agnostic test executable provided by a software unit-testing system. The TestExecutable
 *  is a StorageObjectBase such that its main attributes are accessed via the Glib types and properties system.
 * @see DiscoveryTestExecutable for the non-runnable discovery equivalent.
 */
class TestExecutable : public StorageObjectBase
{
public:
    /**
     * @brief Create a new TargetTestExecutableBase with a name.
     * @param executable_path The path of the test executable.
     */
    explicit TestExecutable(const Glib::ustring &executable_path);

    /**
     * @brief TargetTestExecutableBase
     * @param executable_path The path of the test executable.
     * @param cobject Glib C object
     * @param builder Gtk::Builder template
     */
    TestExecutable(
            const Glib::ustring &executable_path, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    bool operator==(const TestExecutable & other) const noexcept;

private:
    Glib::RefPtr<Gio::FileMonitor> file_monitor;
    std::unordered_set<std::shared_ptr<TestResult>> received_blob;
    bool is_outdated = false;
};

} // namespace optifol

#endif // TARGETTESTEXECUTABLE_HPP
