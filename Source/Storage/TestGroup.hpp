/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the TestGroup grouping GLib object
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef TESTGROUP_HPP
#define TESTGROUP_HPP

#include "../GUI/ProcessExecutor.hpp"
#include "RequirementGroupBase.hpp"
#include "StorageObjectBase.hpp"

namespace optifol
{

class TestGroup : public StorageObjectBase,
                  public RequirementGroupBase
{
    std::unordered_map<std::string, std::unordered_set<Glib::RefPtr<Requirement>>> grouped_executables;

public:
    explicit TestGroup(const Glib::ustring &name);

    TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    decltype(grouped_executables)::const_iterator begin_executable_groups() const noexcept;

    decltype(grouped_executables)::const_iterator end_executable_groups() const noexcept;

    decltype(grouped_executables)::size_type executable_groups_count() const noexcept;

private:
    void handle_requirement_change(guint initial_index, guint removed_count, guint added_count);

    void record_slated_requirement(Glib::RefPtr<Requirement> slated_requirement, guint old_index) override;

    std::unordered_map<guint, Glib::RefPtr<Requirement>> deleted_requirements;
};

} // namespace optifol

#endif // TESTGROUP_HPP
