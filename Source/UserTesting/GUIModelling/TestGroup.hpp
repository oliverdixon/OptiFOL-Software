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
#include "../../Storage/RequirementGroupBase.hpp"
#include "../../Storage/StorageObjectBase.hpp"

namespace optifol
{

class TestGroup : public StorageObjectBase,
                  public RequirementGroupBase,
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

    void record_slated_requirement(Glib::RefPtr<Requirement> slated_requirement, guint old_index) override;

    static Glib::RefPtr<Gio::ListModel> get_expanded_test_list(const Glib::RefPtr<Glib::ObjectBase> &item)
    {
        const auto candidate = std::dynamic_pointer_cast<Requirement>(item);

        if (candidate != nullptr)
            return candidate->get_tests();

        return nullptr;
    }

    Glib::RefPtr<Gtk::TreeListModel> tests_tree =
            Gtk::TreeListModel::create(model, &ITestModelNode::get_given_tree, true);
    std::unordered_map<guint, Glib::RefPtr<Requirement>> deleted_requirements;
};

} // namespace optifol

#endif // TESTGROUP_HPP
