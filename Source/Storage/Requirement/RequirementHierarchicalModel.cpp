/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/1/25.
//

#include "RequirementHierarchicalModel.hpp"

namespace optifol
{

void RequirementHierarchicalModel::register_requirement(Glib::RefPtr<Requirement> &&requirement)
{
    append(requirement);
}

Glib::RefPtr<Requirement> RequirementHierarchicalModel::get_requirement(const Requirement &requirement)
{
    const auto list_index = find_requirement_position(requirement);
    if (list_index.has_value())
        return get_typed_object<Requirement>(*list_index);

    return {};
}

Glib::RefPtr<Requirement> RequirementHierarchicalModel::get_requirement(const std::size_t requirement_id)
{
    const auto list_index = find_requirement_position(requirement_id);
    if (list_index.has_value())
        return get_typed_object<Requirement>(*list_index);

    return {};
}

void RequirementHierarchicalModel::remove_requirement(const Requirement &requirement)
{
    const auto list_index = find_requirement_position(requirement);
    if (list_index.has_value())
        remove(*list_index);
}

void RequirementHierarchicalModel::remove_requirement(const std::size_t requirement_id)
{
    const auto list_index = find_requirement_position(requirement_id);
    if (list_index.has_value())
        remove(*list_index);
}

std::optional<guint> RequirementHierarchicalModel::find_requirement_position(const Requirement &requirement) const
{
    return find_requirement_position(requirement.get_controller_id());
}

std::optional<guint> RequirementHierarchicalModel::find_requirement_position(const std::size_t requirement_id) const
{
    auto [found, position] = find(dummy_requirement,
        [requirement_id](const Glib::RefPtr<const Requirement> &candidate,
                const Glib::RefPtr<const Requirement> &dummy) -> auto
        {
            std::ignore = dummy;
            return candidate->get_controller_id() == requirement_id;
        }
    );

    if (found)
        return position;

    return {};
}

}
