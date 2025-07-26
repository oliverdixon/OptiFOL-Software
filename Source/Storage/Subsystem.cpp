/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the subsystem-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#include <cassert>

#include "Subsystem.hpp"
#include "../Logging.hpp"

namespace optifol
{

Subsystem::Subsystem(const Glib::ustring& name, TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    TreeNode(parent),
    RequirementGroupBase(sigc::mem_fun(*this, &Subsystem::handle_requirement_change))
{
    setup_groups(name);
}

Subsystem::Subsystem(const Glib::ustring& name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder,
        TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    StorageObjectBase(cobject, builder),
    TreeNode(parent),
    RequirementGroupBase(sigc::mem_fun(*this, &Subsystem::handle_requirement_change))
{
    setup_groups(name);
}

bool Subsystem::operator==(const Subsystem &other) const noexcept
{
    return hash() == other.hash();
}

std::string Subsystem::get_path() const
{
    const auto hash = std::hash<Subsystem>{}(*this);
    if (hash != fully_qualified_path_cache.first) {
        fully_qualified_path_cache.first = hash;
        fully_qualified_path_cache.second = get_parent()->get_path() + '/' + property_name().get_value();
    }

    return fully_qualified_path_cache.second;
}

void Subsystem::record_slated_requirement(const Glib::RefPtr<Requirement> slated_requirement, const guint old_index)
{
    deleted_requirements[old_index] = slated_requirement;
}

void Subsystem::duplicate_requirement(const Requirement &requirement)
{
    build_requirement(requirement.property_name().get_value(), requirement.property_statement().get_value(),
            requirement.property_description().get_value(), requirement.property_priority().get_value(),
            requirement.property_test_input().get_value());
}

Glib::RefPtr<Gio::ListStore<AnalysisGroup>> Subsystem::get_analysis_groups() const noexcept
{
    return analysis_groups;
}

Glib::RefPtr<Gio::ListStore<TestGroup>> Subsystem::get_test_groups() const noexcept
{
    return test_groups;
}

void Subsystem::setup_groups(const Glib::ustring &name)
{
    assert(analysis_groups->get_n_items() == 0);
    assert(test_groups->get_n_items() == 0);

    property_name().set_value(name);

    analysis_groups->append(Glib::make_refptr_for_instance(new AnalysisGroup("Unassigned Requirements")));
    test_groups->append(Glib::make_refptr_for_instance(new TestGroup("Unassigned Requirements")));

    assert(analysis_groups->get_n_items() == 1);
    assert(test_groups->get_n_items() == 1);
}

void Subsystem::handle_requirement_change(const guint initial_index, const guint removed_count, const guint added_count)
{
    /*
     * First handle removed requirements, making use of the deleted requirements records. Notice that this doesn't use
     * the model at all, as the Requirements have already been erased and moved into the deleted records map.
     */
    // TODO: efficiently remove multiple records with splice. See insertion logic below.

    for (guint remove_count_i = 0; remove_count_i < removed_count; ++remove_count_i) {
        const auto deleted_it = deleted_requirements.find(remove_count_i + initial_index);

        if (deleted_it == deleted_requirements.cend())
            throw std::runtime_error("The deleted Requirement previously at index " +
                std::to_string(remove_count_i + initial_index) +
                " is not present in the deletion records; cannot propagate to Requirement groups.");

        // Remove from analysis groups.
        if (deleted_it->second->is_analysis_ready()) {
            const auto analysis_group_count = analysis_groups->get_n_items();
            for (guint analysis_group_index = 0; analysis_group_index < analysis_group_count; ++analysis_group_index)
                analysis_groups->get_item(analysis_group_index)->delete_requirement(deleted_it->second);
        }

        // Remove from test groups.
        if (deleted_it->second->observe_test().has_value()) {
            const auto test_group_count = test_groups->get_n_items();
            for (guint test_group_index = 0; test_group_index < test_group_count; ++test_group_index)
                test_groups->get_item(test_group_index)->delete_requirement(deleted_it->second);
        }

        /*
         * Remove from the deleted requirements record, potentially pulling the ref-count to zero and deleting the
         * Requirement from memory.
         */
        deleted_requirements.erase(deleted_it);
    }

    // Then handle new requirements by distributing across the default analysis and test groups.

    const auto default_analysis_group = analysis_groups->get_item(0);
    const auto default_test_group = test_groups->get_item(0);

    if (added_count == 1) {
        const auto candidate = get_requirement(initial_index);

        // Distribute to default analysis group, if it has an OK formula.
        if (candidate->is_analysis_ready())
            default_analysis_group->insert_requirement(candidate);

        // Distribute to default test group, if it has a test.
        if (candidate->observe_test().has_value())
            default_test_group->insert_requirement(candidate);
    }

    else if (added_count > 1) {
        // As above, but specialised for multiple insertion using Gtk's splice capability.

        std::vector<Glib::RefPtr<Requirement>> analysis_additions;
        std::vector<Glib::RefPtr<Requirement>> testing_additions;

        analysis_additions.reserve(added_count);
        testing_additions.reserve(added_count);

        for (guint added_list_i = initial_index; added_list_i < added_count; ++added_list_i) {
            const auto candidate = get_requirement(added_list_i + initial_index);

            if (candidate->is_analysis_ready())
                analysis_additions.push_back(candidate);

            if (candidate->observe_test().has_value())
                testing_additions.push_back(candidate);
        }

        default_analysis_group->insert_requirement(analysis_additions);
        default_test_group->insert_requirement(testing_additions);
    }
}

} // namespace optifol
