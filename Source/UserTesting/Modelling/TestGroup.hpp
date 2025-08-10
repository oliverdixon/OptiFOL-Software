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

#include "../../Storage/ObjectGroup.hpp"
#include "../../Storage/Requirement.hpp"
#include "ExecutionGroup.hpp"

namespace optifol
{

class TestGroup : public StorageObjectBase,
                  public ObjectGroup<Requirement>,
                  public ITestModelNode
{
    std::unordered_set<std::unique_ptr<ExecutionGroup>, std::hash<ExecutionGroup>,
        ExecutionGroupEqualityFunctor> execution_groups;

public:
    explicit TestGroup(const Glib::ustring &name);

    TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    [[nodiscard]] bool operator==(const TestGroup & other) const noexcept;

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_tests_tree() const noexcept override;

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_results_tree() const noexcept override;

    decltype(execution_groups)::const_iterator begin_execution_groups() const noexcept;

    decltype(execution_groups)::const_iterator end_execution_groups() const noexcept;

    /**
     * @brief Bind the name of the TestGroup to a Gtk::Label.
     * @param item A Gtk::ListItem with a Gtk::Label child and TestGroup item.
     */
    static void bind_name_to_label(const Glib::RefPtr<Gtk::ListItem>& item) noexcept;

private:
    void handle_requirement_model_change(guint initial_index, guint removed_count, guint added_count) noexcept;

    // TODO: document. Should also be noexcept, but need to verify downstream.
    void deregister_test_results(Glib::RefPtr<Test> test);

    void deregister_test_executable(const Glib::RefPtr<Test> &test);

    void register_test_results(const Glib::RefPtr<Test>& test);

    /**
     * @brief Allocate the incoming Test to the suitable ExecutionGroup, determined by the Test's target executable
     *  property. A new ExecutionGroup is created if one does not already exist.
     * @param test The Test to register in the execution model.
     * @throws std::runtime_error if the Test could not be added.
     */
    void register_test_executable(const Glib::RefPtr<Test>& test);

    void handle_test_deletions(guint initial_index, guint removed_count) noexcept;

    void handle_test_additions(guint initial_index, guint added_count) noexcept;

    void handle_incoming_result(const std::shared_ptr<Test> &owning_test) noexcept;

    static const log4cxx::LoggerPtr testgroup_logger;

    Glib::RefPtr<Gtk::TreeListModel> tests_tree =
            Gtk::TreeListModel::create(get_model(), &ITestModelNode::get_given_tests_tree, true);

    ObjectGroup<Test> results_model;

    std::unordered_map<Glib::RefPtr<Test>, sigc::connection, std::hash<Test>,
        DereferencingEqualityFunctor<Glib::RefPtr<Test>, Test>> registered_callbacks;

    Glib::RefPtr<Gtk::TreeListModel> results_tree =
            Gtk::TreeListModel::create(results_model.get_model(), &ITestModelNode::get_given_results_tree, true);
};

} // namespace optifol

#endif // TESTGROUP_HPP
