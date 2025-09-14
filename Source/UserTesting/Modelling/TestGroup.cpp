/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the TestGroup grouping GLib object
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#include <gtkmm/label.h>
#include <gtkmm/listitem.h>
#include <cassert>

#include "TestGroup.hpp"
#include "../../Exceptions/SemanticException.hpp"
#include "../Logging.hpp"
#include "GoogleExecutionGroup.hpp"

namespace optifol
{

const log4cxx::LoggerPtr TestGroup::testgroup_logger = Logging::get_logger({"GUI", "StorageControl", "TestGroup"});

TestGroup::TestGroup(const Glib::ustring &name) :
    Glib::ObjectBase("TestGroup"),
    ObjectGroup(sigc::mem_fun(*this, &TestGroup::handle_requirement_model_change))
{
    property_name().set_value(name);
}

TestGroup::TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("TestGroup"),
    StorageObjectBase(cobject, builder),
    ObjectGroup(sigc::mem_fun(*this, &TestGroup::handle_requirement_model_change))
{
    property_name().set_value(name);
}

bool TestGroup::operator==(const TestGroup &other) const noexcept
{
    return property_name().get_value() == other.property_name().get_value();
}

Glib::RefPtr<Gtk::TreeListModel> TestGroup::get_tests_tree() const noexcept
{
    return tests_tree;
}

Glib::RefPtr<Gtk::TreeListModel> TestGroup::get_results_tree() const noexcept
{
    return results_tree;
}

decltype(TestGroup::execution_groups)::const_iterator TestGroup::begin_execution_groups() const noexcept
{
    return execution_groups.cbegin();
}

decltype(TestGroup::execution_groups)::const_iterator TestGroup::end_execution_groups() const noexcept
{
    return execution_groups.cend();
}

void TestGroup::bind_name_to_label(const Glib::RefPtr<Gtk::ListItem> &item) noexcept
{
    const auto target_label = dynamic_cast<Gtk::Label *>(item->get_child());
    const auto typed_group = dynamic_cast<const TestGroup *>(item->get_item().get());
    if (target_label == nullptr || typed_group == nullptr)
        return;

    target_label->set_text(typed_group->property_name().get_value());
}

void TestGroup::handle_requirement_model_change(const guint initial_index, const guint removed_count,
    const guint added_count) noexcept
{
    testgroup_logger->debug("Handling requirements change: " + std::to_string(added_count) + " additions and " +
            std::to_string(removed_count) + " deletions at position " + std::to_string(initial_index) + '.');

    handle_test_deletions(initial_index, removed_count);
    handle_test_additions(initial_index, added_count);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from 'handle_test_deletions' callback.
void TestGroup::deregister_test_results(Glib::RefPtr<Test> test)
{
    assert(test != nullptr);
    const auto callback_it = registered_callbacks.find(test);

    if (callback_it == registered_callbacks.cend())
        testgroup_logger->error("Removed Test \"" + test->property_name().get_value() +
            "\" did not have a registered callback in Test Group \"" + property_name().get_value() +
            "\".");
    else {
        callback_it->second.disconnect();
        registered_callbacks.erase(callback_it);
        testgroup_logger->info("Disconnected results signal handler for Test \"" +
            test->property_name().get_value() + "\" in Test Group \"" + property_name().get_value()
            + "\".");
    }

    if (results_model.delete_object(std::move(test)))
        testgroup_logger->info("Removed Test \"" + test->property_name().get_value() +
            "\" from the failed results model.");
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from 'handle_test_deletions' callback.
void TestGroup::deregister_test_executable(const Glib::RefPtr<Test> &test)
{
    assert(test != nullptr);

#if 0 // TODO URGENT HERE OWD
    /*
     * Locate the ExecutionGroup that should contain the Test, according to its TargetExecutable, and remove it from
     * the ExecutionGroup. If there is no suitable ExecutionGroup, then that is considered an error as all tests have
     * associated TargetExecutables, and should have been allocated appropriately upon being registered with the
     * TestGroup.
     */
    const auto exe_group_it = execution_groups.find(*test->observe_test_executable());
    if (exe_group_it == execution_groups.cend())
        testgroup_logger->error("Test \"" + test->property_name().get_value() +
            "\" was not a member of any Execution Groups in \"" + property_name().get_value() + "\".");
    else {
        (*exe_group_it)->remove_test(test);
        testgroup_logger->info("Removed Test \"" + test->property_name().get_value() +
            "\" from Execution Group for \"" + (*exe_group_it)->get_executable_name() + "\".");
    }

    // If the execution group is now empty, remove it.
    if ((*exe_group_it)->is_empty()) {
        testgroup_logger->info("Removed empty Execution Group for \"" + (*exe_group_it)->get_executable_name() + "\".");
        execution_groups.erase(exe_group_it);
    }
#endif
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from 'handle_test_additions' callback.
void TestGroup::register_test_results(const Glib::RefPtr<Test> &test)
{
    assert(test != nullptr);

    /*
     * Register a results callback so we're informed each time a new TestResult is emplaced into the Test. This needs to
     * be recorded so it can be disconnected when the test is deleted. Note that TestGroups do not permit duplicates, so
     * a regular hash table, keyed on the Test, is acceptable.
     */
    registered_callbacks.emplace(
        test,
        test->property_result().signal_changed().connect(
            sigc::bind(sigc::mem_fun(*this, &TestGroup::handle_incoming_result), test)
        )
    );

    testgroup_logger->info("Connected results signal handler for Test \"" +
        test->property_name().get_value() + "\" in Test Group \"" + property_name().get_value() + "\".");
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from 'handle_test_additions' callback.
void TestGroup::register_test_executable(const Glib::RefPtr<Test> &test)
{
    assert(test != nullptr);

#if 0 // TODO URGENT HERE OWD
    try {
        const auto group_it = execution_groups.find(*test->observe_test_executable());
        if (group_it == execution_groups.cend()) {
            // The first test we've seen using this executable. Create a new execution group.
            execution_groups.emplace(std::make_unique<GoogleExecutionGroup>(test));
            testgroup_logger->info("Created new Execution Group for executable \"" +
                test->observe_test_executable()->property_name().get_value() + "\".");
        } else
            // We've seen this executable before. Add it to the existing execution group.
            group_it->get()->add_test(test);

        testgroup_logger->info("Added Test \"" + test->property_name().get_value() +
            "\" to Execution Group for \"" + test->observe_test_executable()->property_name().get_value() +
            "\".");

    } catch (const SemanticException& semantic_exception) {
        testgroup_logger->error("Could not propagate Test addition for Test \"" +
            test->property_name().get_value() + "\".");
        testgroup_logger->error(semantic_exception.what());
    }
#endif
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive. Called from 'handle_object_change'.
void TestGroup::handle_test_deletions(const guint initial_index, const guint removed_count) noexcept
{
    for (guint remove_count_i = 0; remove_count_i < removed_count; ++remove_count_i)
        // Get the deletion record, keyed by the old index, to recover the deleted Requirement and Test.
        try {
            const auto deleted_it = steal_deleted_object(initial_index + remove_count_i);
            const auto deleted_tests = deleted_it->get_tests();
            const auto deleted_test_count = deleted_tests->get_n_items();

            if (deleted_test_count == 0)
                continue; // Nothing to do...

            for (guint deleted_test_index = 0; deleted_test_index < deleted_test_count; ++deleted_test_index) {
                const auto test = deleted_tests->get_item(deleted_test_index);
                deregister_test_executable(test);
                deregister_test_results(test);
            }
        } catch (const std::runtime_error& global_error) {
            testgroup_logger->error("Could not propagate any deletions of " + std::to_string(removed_count) +
                " from index " + std::to_string(initial_index) + " for Test Group " + property_name().get_value() +
                "\".");
            testgroup_logger->error(global_error.what());
        }
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive. Called from 'handle_object_change'.
void TestGroup::handle_test_additions(const guint initial_index, const guint added_count) noexcept
{
    for (guint added_count_i = 0; added_count_i < added_count; ++added_count_i)
        try {
            // Get the Tests from the incoming Requirement.
            const auto& tests = get_object_by_index(added_count_i + initial_index)->get_tests();
            const auto added_test_count = tests->get_n_items();

            // For each Test, register a results handler and allocate to the suitable execution group.
            for (guint added_test_index = 0; added_test_index < added_test_count; ++added_test_index) {
                const auto test = tests->get_item(added_test_index);
                register_test_results(test);
                register_test_executable(test);
            }

        } catch (const std::runtime_error& global_error) {
            testgroup_logger->error("Could not propagate any additions of " + std::to_string(added_count) +
                " from index " + std::to_string(initial_index) + " for Test Group " + property_name().get_value() +
                "\".");
            testgroup_logger->error(global_error.what());
        }
}

void TestGroup::handle_incoming_result(const std::shared_ptr<Test> &owning_test) noexcept
{
    if (owning_test == nullptr) {
        testgroup_logger->error("Test Group \"" + property_name().get_value() +
            "\" was informed on result of non-existent Test.");
        return;
    }

    const auto test_already_exists = results_model.contains_exact(owning_test.get());
    const auto result = owning_test->property_result().get_value().get();

    if (result == nullptr || result->get_results_tree()->get_n_items() == 0) {
        if (test_already_exists) {
            results_model.delete_object(owning_test);
            testgroup_logger->info("Removed Test \"" + owning_test->property_name().get_value() +
                "\" from the failed model for the Test Group \"" + property_name().get_value() + "\".");
        }

        return;
    }

    // The test has partial results, so add it to the results model if it doesn't already exist.
    if (test_already_exists == false) {
        try {
            results_model.insert_object(owning_test);
            testgroup_logger->info("Added Test \"" + owning_test->property_name().get_value() +
                "\" to the failed model for the Test Group \"" + property_name().get_value() + "\".");
        } catch (const std::runtime_error& error) {
            testgroup_logger->error("Could not add Test \"" + owning_test->property_name().get_value() +
                "\" to the failed model for the Test Group \"" + property_name().get_value() + "\".");
            testgroup_logger->error(error.what());
        }
    } else
        testgroup_logger->debug("Received valid failure result for Test \"" + owning_test->property_name().get_value() +
            "\", but it already exists in the model for Test Group \"" + property_name().get_value() + "\".");
}

} // namespace optifol
