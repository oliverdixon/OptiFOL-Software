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

/**
 * @class TestGroup
 * @brief A TestGroup is a user-populated structure that groups Test objects.
 * @details
 *  Internally, a TestGroup maintains a number of models that must be synchronised. Most of the implementation
 * of TestGroup is focused on keeping these models synchronised through a series of callbacks. There are three
 * models of note: <ol> <li> <b>The Requirement model.</b> Provided by the ObjectGroup<Requirement> base
 * class, the TestGroup groups Requirement objects endowed with Test objects. This may seem counter-intuitive,
 * but it is desired behaviour. It means that Test objects must be added via their constituent Requirement
 * parents, which allows the TestGroup to directly expose Test instances grouped according to the
 * corresponding Requirement. As the ObjectGroup CRTP base class is being used, Requirement objects are
 * transparently held in both a linear model (iteration) and a hashed model (lookup) that are invariantly
 * synchronised.
 *      </li>
 *      <li>
 *          <b>The ExecutionGroup model.</b> Provided by an implementation detail, ExecutionGroup objects
 * aggregate Test objects (irrespective of Requirement) according to their associated TestExecutable.
 * Therefore, in addition to a model centred on Requirement, there is a model centred on TestExecutable.
 * ExecutionGroup objects can be accessed via the iterator-based member functions @ref begin_execution_groups
 * and
 *          @ref end_execution_groups.
 *      </li>
 *      <li>
 *          <b>The TestResult and PartialTestResult model.</b> Provided by ObjectGroup<Test>, the TestGroup
 * groups TestResult and PartialTestResult objects based on the Test to which they refer, irrespective of
 *          Requirement or TestExecutable. This enables a second 'view' of the TestGroup, accessible via the
 *          ITestModelNode member functions, displaying TestResult information without regard to the
 * Requirement structure.
 *      </li>
 *  </ol>
 */
class TestGroup : public StorageObjectBase,
                  public ObjectGroup<Requirement>,
                  public ITestModelNode
{
    UniqueUnorderedSet<ExecutionGroup> execution_groups;

public:
    /**
     * @brief Constructs a new TestGroup with the given name
     * @param name The initial name of the TestGroup
     */
    explicit TestGroup(const Glib::ustring &name);

    /**
     * @brief Constructs a new TestGroup with the given name
     * @param name The initial name of the TestGroup
     * @param cobject The base C GTK object
     * @param builder The source GTK builder object
     */
    TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    /**
     * @brief Determines equality between two TestGroup objects based on their name
     * @param other The TestGroup with which equality should be tested.
     * @return Is the object equivalent to the given TestGroup?
     */
    [[nodiscard]] bool operator==(const TestGroup &other) const noexcept;

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_tests_tree() const noexcept override;

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_results_tree() const noexcept override;

    /**
     * @brief Get the constant iterator to the beginning of the ordered execution groups.
     * @return Constant iterator to the beginning of the execution groups.
     */
    decltype(execution_groups)::const_iterator begin_execution_groups() const noexcept;

    /**
     * @brief Get the constant iterator to the one-past-end of the ordered execution groups.
     * @return Constant iterator to the one-past-end of the execution groups.
     */
    decltype(execution_groups)::const_iterator end_execution_groups() const noexcept;

    /**
     * @brief Bind the name of the TestGroup to a Gtk::Label.
     * @param item A Gtk::ListItem with a Gtk::Label child and TestGroup item.
     */
    static void bind_name_to_label(const Glib::RefPtr<Gtk::ListItem> &item) noexcept;

private:
    /**
     * @brief Handles a change to the underlying object model by registering and/or de-registering Test
     * objects from Requirement objects.
     * @param initial_index The initial index in the list model from which changes were made.
     * @param removed_count The number of removed items at the initial index.
     * @param added_count The number of inserted items at the initial index.
     * @see handle_test_deletions for the deletion handler
     * @see handle_test_additions for the addition handler
     */
    void handle_requirement_model_change(
            guint initial_index, guint removed_count, guint added_count) noexcept;

    /**
     * @brief Remove the TestResult model entries for the given Test.
     * @param test The slated Test associated with the TestResult model entries to de-register.
     * @see @ref results_model for the model.
     * @pre The given Test container is non-null.
     */
    void deregister_test_results(Glib::RefPtr<Test> test);

    /**
     * @brief Remove the given Test from the corresponding ExecutionGroup within the group model. If the
     * removal renders the corresponding ExecutionGroup empty, it is removed from the model entirely.
     * @param test The Test to remove from the ExecutionGroup.
     * @see @ref execution_groups for the model.
     * @pre The given Test container is non-null.
     */
    void deregister_test_executable(const Glib::RefPtr<Test> &test);

    /**
     * @brief Register a new callback to listen for changes to the TestResult objects assigned to the given
     * Test, and synchronise the changes in our local @ref results_model.
     * @param test The Test whose results to watch.
     * @pre The given Test container is non-null.
     */
    void register_test_results(const Glib::RefPtr<Test> &test);

    /**
     * @brief Allocate the incoming Test to the suitable ExecutionGroup, determined by the Test's target
     * executable property. A new ExecutionGroup is created if one does not already exist.
     * @param test The Test to register in the execution model.
     * @throws std::runtime_error if the Test could not be added.
     * @pre The given Test container is non-null.
     */
    void register_test_executable(const Glib::RefPtr<Test> &test);

    /**
     * @brief Handle Test deletions by deregistering watchers for TestResult changes, and removing from the
     *  ExecutionGroup model.
     * @param initial_index The initial index of the deleted Test objects; see @ref
     * ObjectGroup::steal_deleted_object.
     * @param removed_count The number of removed Test objects from the initial index.
     * @see deregister_test_executable for removal from ExecutionGroup model.
     * @see deregister_test_results for removal from TestResult model.
     */
    void handle_test_deletions(guint initial_index, guint removed_count) noexcept;

    /**
     * @brief Handle Tets additions by registering watchers for TestResult changes, and inserting into the
     *  ExecutionGroup model.
     * @param initial_index The initial index of the inserted Test objects; see @ref
     * ObjectGroup::get_object_by_index.
     * @param added_count The number of inserted Test objects from the initial index.
     * @see register_test_results for insertion into TestResult model.
     * @see register_test_executable for insertion into ExecutionGroup model.
     */
    void handle_test_additions(guint initial_index, guint added_count) noexcept;

    /**
     * @brief Handle a new TestResult object for the given Test.
     * @param owning_test The Test containing the new result.
     */
    void handle_incoming_result(const std::shared_ptr<Test> &owning_test) noexcept;

    static const log4cxx::LoggerPtr testgroup_logger;

    Glib::RefPtr<Gtk::TreeListModel> tests_tree =
            Gtk::TreeListModel::create(get_model(), &ITestModelNode::get_given_tests_tree, true);

    ObjectGroup<Test> results_model;

    SharedUnorderedMap<Test, sigc::connection> registered_callbacks;

    Glib::RefPtr<Gtk::TreeListModel> results_tree = Gtk::TreeListModel::create(
            results_model.get_model(), &ITestModelNode::get_given_results_tree, true);
};

} // namespace optifol

#endif // TESTGROUP_HPP
