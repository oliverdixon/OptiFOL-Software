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

#include "../../GUI/ProcessExecutor.hpp"
#include "../../Storage/ObjectGroupBase.hpp"
#include "../../Storage/Requirement.hpp"

namespace optifol
{

class TestGroup : public StorageObjectBase,
                  public ObjectGroupBase<Requirement>,
                  public ITestModelNode
{
    std::unordered_map<std::string, std::unordered_set<Glib::RefPtr<Requirement>>> grouped_executables;

public:
    explicit TestGroup(const Glib::ustring &name);

    TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_tree() const noexcept override;

    decltype(grouped_executables)::const_iterator begin_executable_groups() const noexcept;

    decltype(grouped_executables)::const_iterator end_executable_groups() const noexcept;

private:
    void handle_requirement_change(guint initial_index, guint removed_count, guint added_count);

    Glib::RefPtr<Gtk::TreeListModel> tests_tree =
            Gtk::TreeListModel::create(model, &ITestModelNode::get_given_tree, true);
};

} // namespace optifol

#endif // TESTGROUP_HPP
