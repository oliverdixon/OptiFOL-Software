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

#include "../Logging.hpp"
#include "GoogleExecutionGroup.hpp"

namespace optifol
{

const log4cxx::LoggerPtr TestGroup::testgroup_logger = Logging::get_logger({"GUI", "StorageControl", "TestGroup"});

TestGroup::TestGroup(const Glib::ustring &name) :
    Glib::ObjectBase("TestGroup"),
    ObjectGroupBase(sigc::mem_fun(*this, &TestGroup::handle_test_change))
{
    property_name().set_value(name);
}

TestGroup::TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("TestGroup"),
    StorageObjectBase(cobject, builder),
    ObjectGroupBase(sigc::mem_fun(*this, &TestGroup::handle_test_change))
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

void TestGroup::handle_test_change(const guint initial_index, const guint removed_count, const guint added_count)
{
    testgroup_logger->debug("Handling requirements change: " + std::to_string(added_count) + " additions and " +
            std::to_string(removed_count) + " deletions at position " + std::to_string(initial_index) + '.');

    handle_test_deletions(initial_index, removed_count);
    handle_test_additions(initial_index, added_count);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive. Called from 'handle_test_change'.
void TestGroup::handle_test_deletions(guint initial_index, guint removed_count)
{
#if 0 // TODO URGENT
    // First remove all deleted Requirements from the grouped executable map.

    for (guint remove_count_i = 0; remove_count_i < removed_count; ++remove_count_i) {
        // Get the deletion record, keyed by the old index, to recover the deleted Requirement.
        const auto deleted_it = deleted_requirements.find(remove_count_i + initial_index);
        if (deleted_it == deleted_requirements.cend())
            throw std::runtime_error("The deleted Requirement previously at index " +
                std::to_string(remove_count_i + initial_index) + " is not present in the deletion records.");

        // Recover the Test object from the deleted Requirement.
        const auto& deleted_req_test = deleted_it->second->observe_test();
        if (deleted_req_test.has_value() == false)
            throw std::runtime_error("The deleted Requirement \"" + deleted_it->second->property_name().get_value() +
                "\" was present in the Test Group deletion records but did not have an associated Test.");

        // Locate the group in which the Requirement was stored, keyed by its target executable.
        const auto executable_group_it = grouped_executables.find(deleted_req_test->property_target_executable_name().
            get_value());

        if (executable_group_it != grouped_executables.cend()) {
            // Locate the Requirement within the group and erase it.
            const auto group_member_it = executable_group_it->second.find(deleted_it->second);
            if (group_member_it != executable_group_it->second.cend())
                executable_group_it->second.erase(group_member_it);

            // If the group is now empty, the entire thing can be deleted.
            if (executable_group_it->second.empty())
                grouped_executables.erase(executable_group_it);
        }

        deleted_requirements.erase(deleted_it);
    }
#endif
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive. Called from 'handle_test_change'.
void TestGroup::handle_test_additions(const guint initial_index, const guint added_count)
{
    for (guint added_count_i = 0; added_count_i < added_count; ++added_count_i) {
        // Get the Tests from the incoming Requirement.
        const auto& tests = get_object_by_index(added_count_i + initial_index)->get_tests();
        const auto test_count = tests->get_n_items();

        // For each test, add it to the corresponding execution group.
        for (guint test_index = 0; test_index < test_count; ++test_index) {
            const auto test = tests->get_item(test_index);
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
                "\" to Execution Group for \"" +  test->observe_test_executable()->property_name().get_value() + "\".");
        }
    }
}

} // namespace optifol
