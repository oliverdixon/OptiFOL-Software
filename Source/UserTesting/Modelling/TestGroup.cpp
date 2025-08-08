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

#include "TestGroup.hpp"

#include "../../Exceptions/SemanticException.hpp"
#include "../Logging.hpp"
#include "GoogleExecutionGroup.hpp"

namespace optifol
{

const log4cxx::LoggerPtr TestGroup::testgroup_logger = Logging::get_logger({"GUI", "StorageControl", "TestGroup"});

TestGroup::TestGroup(const Glib::ustring &name) :
    Glib::ObjectBase("TestGroup")
{
    property_name().set_value(name);
}

TestGroup::TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("TestGroup"),
    StorageObjectBase(cobject, builder)
{
    property_name().set_value(name);
}

Glib::RefPtr<Gtk::TreeListModel> TestGroup::get_tree() const noexcept
{
    return tests_tree;
}

decltype(TestGroup::execution_groups)::const_iterator TestGroup::begin_execution_groups() const noexcept
{
    return execution_groups.cbegin();
}

decltype(TestGroup::execution_groups)::const_iterator TestGroup::end_execution_groups() const noexcept
{
    return execution_groups.cend();
}

void TestGroup::handle_object_change(const guint initial_index, const guint removed_count,
    const guint added_count) noexcept
{
    testgroup_logger->debug("Handling requirements change: " + std::to_string(added_count) + " additions and " +
            std::to_string(removed_count) + " deletions at position " + std::to_string(initial_index) + '.');

    handle_test_deletions(initial_index, removed_count);
    handle_test_additions(initial_index, added_count);
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

            // Remove all requirement tests from all execution groups.
            for (auto exe_group_it = execution_groups.cbegin(); exe_group_it != execution_groups.cend(); ) {
                for (guint deleted_test_index = 0; deleted_test_index < deleted_test_count; ++deleted_test_index) {
                    const auto test = deleted_tests->get_item(deleted_test_index);
                    (*exe_group_it)->remove_test(test);

                    testgroup_logger->debug("Removed Test \"" + test->property_name().get_value() +
                        "\" from Execution Group for \"" +
                        test->observe_test_executable()->property_name().get_value() + "\".");
                }

                // If the execution group is now empty, remove it.
                if ((*exe_group_it)->is_empty()) {
                    testgroup_logger->debug("Removed empty Execution Group for \"" +
                        (*exe_group_it)->get_executable_name() + "\".");
                    execution_groups.erase(exe_group_it++);
                } else
                    ++exe_group_it;
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
            const auto test_count = tests->get_n_items();

            // For each test, add it to the corresponding execution group.
            for (guint test_index = 0; test_index < test_count; ++test_index) {
                const auto test = tests->get_item(test_index);

                try {
                    const auto group_it = execution_groups.find(*test->observe_test_executable());

                    if (group_it == execution_groups.cend()) {
                        // The first test we've seen using this executable. Create a new execution group.
                        execution_groups.emplace(std::make_unique<GoogleExecutionGroup>(test));
                        testgroup_logger->debug("Created new Execution Group for executable \"" +
                            test->observe_test_executable()->property_name().get_value() + "\".");
                    } else
                        // We've seen this executable before. Add it to the existing execution group.
                        group_it->get()->add_test(test);

                    testgroup_logger->debug("Added Test \"" + test->property_name().get_value() +
                        "\" to Execution Group for \"" + test->observe_test_executable()->property_name().get_value() +
                        "\".");
                } catch (const SemanticException& semantic_exception) {
                    testgroup_logger->error("Could not propagate Test addition for Test \"" +
                        test->property_name().get_value() + "\".");
                    testgroup_logger->error(semantic_exception.what());
                }
            }
        } catch (const std::runtime_error& global_error) {
            testgroup_logger->error("Could not propagate any additions of " + std::to_string(added_count) +
                " from index " + std::to_string(initial_index) + " for Test Group " + property_name().get_value() +
                "\".");
            testgroup_logger->error(global_error.what());
        }
}

} // namespace optifol
