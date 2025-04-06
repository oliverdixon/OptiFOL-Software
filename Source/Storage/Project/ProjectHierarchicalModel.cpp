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

#include "ProjectHierarchicalModel.hpp"

#include <iostream>

namespace optifol
{

void ProjectHierarchicalModel::register_object(Glib::RefPtr<Project> &&project)
{
    hierarchy.emplace(project, Glib::make_refptr_for_instance(
        new SubsystemHierarchicalModel(project->get_controller_id())));
    append(project);
}

Glib::RefPtr<Project> ProjectHierarchicalModel::get_object(const Project &project)
{
    const auto list_index = find_object_position(project);
    if (list_index.has_value())
        return get_typed_object<Project>(*list_index);

    return {};
}

Glib::RefPtr<Project> ProjectHierarchicalModel::get_object(const std::size_t project_id)
{
    const auto list_index = find_object_position(project_id);
    if (list_index.has_value())
        return get_typed_object<Project>(*list_index);

    return {};
}

void ProjectHierarchicalModel::remove_object(const std::size_t project_id)
{
    const auto list_index = find_object_position(project_id);
    if (list_index.has_value()) {
        const auto hierarchy_it = hierarchy.find(project_id);
        if (hierarchy_it != hierarchy.cend())
            hierarchy.erase(hierarchy_it);

        remove(*list_index);
    }
}

Glib::RefPtr<SubsystemHierarchicalModel> ProjectHierarchicalModel::expand_project(const Project &project)
{
    const auto hierarchy_it = hierarchy.find(project);
    if (hierarchy_it == hierarchy.cend())
        return {};

    return hierarchy_it->second;
}

Glib::RefPtr<SubsystemHierarchicalModel> ProjectHierarchicalModel::expand_project(const std::size_t project_id)
{
    const auto hierarchy_it = hierarchy.find(project_id);
    if (hierarchy_it == hierarchy.cend())
        return {};

    return hierarchy_it->second;
}

std::optional<guint> ProjectHierarchicalModel::find_object_position(const Project &project) const
{
    return find_object_position(project.get_controller_id());
}

std::optional<guint> ProjectHierarchicalModel::find_object_position(const std::size_t project_id) const
{
    auto [found, position] = find(dummy_project,
        [project_id](const Glib::RefPtr<const Project> &candidate, const Glib::RefPtr<const Project> &dummy) -> auto
        {
            std::ignore = dummy;
            return candidate->get_controller_id() == project_id;
        }
    );

    if (found)
        return position;

    return {};
}

}
