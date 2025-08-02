/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Requirement grouping base
 * @author Oliver Dixon
 * @date 2025-07-26
 * @version Development
 */

#include <cassert>

#include "RequirementGroupBase.hpp"

namespace optifol
{

RequirementGroupBase::RequirementGroupBase(sigc::slot<void(guint, guint, guint)> &&changed_callback)
{
    model->signal_items_changed().connect(std::move(changed_callback));
}

Glib::RefPtr<Gio::ListModel> RequirementGroupBase::get_tree_model() const noexcept
{
    return model;
}

void RequirementGroupBase::insert_requirement(const Glib::RefPtr<Requirement> &new_requirement)
{
    model->append(new_requirement);
    index_map[new_requirement] = model->get_n_items() - 1;
}

void RequirementGroupBase::insert_requirement(const std::vector<Glib::RefPtr<Requirement>> &new_requirements)
{
    // Append the incoming requirements into the list model.
    const auto previous_count = model->get_n_items();
    model->splice(previous_count, 0, new_requirements);

    // Verify insertion with the constant-time size heuristic.
    const auto new_count = model->get_n_items();
    assert(new_count == previous_count + new_requirements.size());

    // Map each new requirement to its corresponding position in the list model.
    for (guint new_index = previous_count; new_index < new_count; ++new_index)
        index_map[new_requirements[new_count - previous_count]] = new_index - 1;
}

Glib::RefPtr<Requirement> RequirementGroupBase::get_requirement(const guint requirement_idx) const
{
    const auto requirement = model->get_item(requirement_idx);

    if (requirement == nullptr)
        throw std::runtime_error("Requirement with given index does not exist in the Subsystem index.");

    return requirement;
}

void RequirementGroupBase::delete_requirement(const Glib::RefPtr<Requirement> &slated_requirement)
{
    const auto index_it = index_map.find(slated_requirement);
    if (index_it == index_map.cend())
        throw std::runtime_error("Slated Requirement is not in the Requirement Group.");

    record_slated_requirement(slated_requirement, index_it->second);
    model->remove(index_it->second);

    for (auto &it: index_map)
        /*
         * TODO this is very inefficient. We should maintain a shift map to correspond offsets from the index map to be
         *  applied only when required. Should be done with reasonable urgency. We're reindexing the map for every
         *  deletion!
         *
         * Sketch of a solution:
         *
         * Keep an ordered map to maintain a correspondence between the index in the index map ("stored index") and
         * shifts that have been applied to that elements. If you had A, B, and C; and B was deleted; the stored index
         * of C would be 2 but we could record a shift of -1. When the index of C was queried again, we would apply the
         * shift lazily.
         *
         * Of course, changes need to cascade. So if A was also deleted, a shift of -1 would be recorded for B, but the
         * C shift should be updated to -2. When the shift map became sufficiently large, an entire re-index could be
         * done to clear down the shift records.
         *
         * I did attempt this but it was finicky, and wasn't completely clear on the problem I was trying to solve.
         * Needs a second attempt soon.
         */
        if (it.second > index_it->second)
            it.second--;

    index_map.erase(index_it);
}

void RequirementGroupBase::record_slated_requirement(
        const Glib::RefPtr<Requirement> slated_requirement, const guint old_index)
{
    std::ignore = slated_requirement;
    std::ignore = old_index;
}

void RequirementGroupBase::populate_selection_model(Gtk::SingleSelection &target_selection_model) const
{
    target_selection_model.set_model(model);
}

void RequirementGroupBase::for_each(const std::function<void(Requirement &)> &function) const
{
    const auto count = model->get_n_items();
    for (guint index = 0; index < count; ++index)
        function(*model->get_item(index));
}

} // namespace optifol
