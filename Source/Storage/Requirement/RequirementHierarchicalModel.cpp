/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation of the Glib-backed hierarchical-based storable object for projects
 * @author Oliver Dixon
 * @date 2025-04-05
 * @version Development
 */

#include "RequirementHierarchicalModel.hpp"

namespace optifol
{

RequirementHierarchicalModel::RequirementHierarchicalModel(std::size_t relevant_subsystem_tag) :
    relevant_subsystem_tag(relevant_subsystem_tag)
{
}

void RequirementHierarchicalModel::register_object(Glib::RefPtr<Requirement> &&requirement)
{
    if (requirement->get_relevant_subsystem_tag() == relevant_subsystem_tag)
        append(requirement);
}

Glib::RefPtr<Requirement> RequirementHierarchicalModel::get_object(const Requirement &requirement)
{
    const auto list_index = find_object_position(requirement);
    if (list_index.has_value())
        return get_typed_object<Requirement>(*list_index);

    return {};
}

Glib::RefPtr<Requirement> RequirementHierarchicalModel::get_object(const std::size_t requirement_id)
{
    const auto list_index = find_object_position(requirement_id);
    if (list_index.has_value())
        return get_typed_object<Requirement>(*list_index);

    return {};
}

bool RequirementHierarchicalModel::remove_object(const std::size_t requirement_id)
{
    const auto list_index = find_object_position(requirement_id);

    if (list_index.has_value()) {
        remove(*list_index);
        return true;
    }

    return false;
}

std::size_t RequirementHierarchicalModel::get_subsystem_tag() const
{
    return relevant_subsystem_tag;
}

std::optional<guint> RequirementHierarchicalModel::find_object_position(const Requirement &requirement) const
{
    return find_object_position(requirement.get_controller_id());
}

std::optional<guint> RequirementHierarchicalModel::find_object_position(const std::size_t requirement_id) const
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
