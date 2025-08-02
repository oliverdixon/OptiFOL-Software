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

namespace optifol
{

TestGroup::TestGroup(const Glib::ustring &name) :
    Glib::ObjectBase("TestGroup"),
    ObjectGroupBase(sigc::mem_fun(*this, &TestGroup::handle_requirement_change))
{
    property_name().set_value(name);
}

TestGroup::TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("TestGroup"),
    StorageObjectBase(cobject, builder),
    ObjectGroupBase(sigc::mem_fun(*this, &TestGroup::handle_requirement_change))
{
    property_name().set_value(name);
}

Glib::RefPtr<Gtk::TreeListModel> TestGroup::get_tree() const noexcept
{
    return tests_tree;
}

decltype(TestGroup::grouped_executables)::const_iterator TestGroup::begin_executable_groups() const noexcept
{
    return grouped_executables.cbegin();
}

decltype(TestGroup::grouped_executables)::const_iterator TestGroup::end_executable_groups() const noexcept
{
    return grouped_executables.cend();
}

void TestGroup::handle_requirement_change(const guint initial_index, const guint removed_count, const guint added_count)
{
#if 0 // TODO URGENT - We must handle changes in the Requirement model, else the TestGroup will have dangling refs.
    // First remove all deleted Requirements from the grouped executable map.

    for (guint remove_count_i = 0; remove_count_i < removed_count; ++remove_count_i) {
        // Get the deletion record, keyed by the old index, to recover the deletd Requirement.
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

    // Then handle new Requirements by grouping according to the executable name.

    for (guint added_count_i = 0; added_count_i < added_count; ++added_count_i) {
        // Get the Test from the incoming Requirement.
        auto requirement = get_object_by_index(added_count_i + initial_index);
        const auto& requirement_test = requirement->observe_test();

        if (requirement_test.has_value() == false)
            throw std::runtime_error("The incoming Requirement \"" + requirement->property_name().get_value() + "\""
                " does not have an associated Test.");

        // Get the executable group with which the incoming Requirement should be associated, creating if needed.
        const auto& executable_name = requirement_test->property_target_executable_name().get_value();
        auto executable_group_it = grouped_executables.find(executable_name);
        if (executable_group_it == grouped_executables.end())
            executable_group_it = grouped_executables.emplace(executable_name,
                std::unordered_set<Glib::RefPtr<Requirement>>{}).first;

        // Insert the incoming Requirement into the suitable executable group.
        executable_group_it->second.insert(std::move(requirement));
    }
#endif
}

} // namespace optifol
