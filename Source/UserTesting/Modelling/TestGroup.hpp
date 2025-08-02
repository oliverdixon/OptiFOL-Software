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

#include "../../Storage/ObjectGroupBase.hpp"
#include "../../Storage/Requirement.hpp"
#include "ExecutionGroup.hpp"

namespace optifol
{

class TestGroup : public StorageObjectBase,
                  public ObjectGroupBase<Requirement>,
                  public ITestModelNode
{
    std::unordered_set<std::unique_ptr<ExecutionGroup>, std::hash<ExecutionGroup>,
        ExecutionGroupEqualityFunctor> execution_groups;

public:
    explicit TestGroup(const Glib::ustring &name);

    TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_tree() const noexcept override;

    decltype(execution_groups)::const_iterator begin_execution_groups() const noexcept;

    decltype(execution_groups)::const_iterator end_execution_groups() const noexcept;

private:
    static const log4cxx::LoggerPtr testgroup_logger;

    void handle_test_change(guint initial_index, guint removed_count, guint added_count);

    void handle_test_deletions(guint initial_index, guint removed_count);

    void handle_test_additions(guint initial_index, guint added_count);

    Glib::RefPtr<Gtk::TreeListModel> tests_tree =
            Gtk::TreeListModel::create(model, &ITestModelNode::get_given_tree, true);
};

} // namespace optifol

#endif // TESTGROUP_HPP
