/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/1/25.
//

#include "ProjectHierarchicalModel.hpp"

#include <iostream>

namespace optifol
{

void ProjectHierarchicalModel::register_project(Glib::RefPtr<Project> &&project)
{
    hierarchy.emplace(project, Glib::make_refptr_for_instance(new SubsystemHierarchicalModel()));
    append(project);
}

Glib::RefPtr<Project> ProjectHierarchicalModel::get_project(const Project &project)
{
    const auto list_index = find_project_position(project);
    if (list_index.has_value())
        return get_typed_object<Project>(*list_index);

    return {};
}

Glib::RefPtr<Project> ProjectHierarchicalModel::get_project(const std::size_t project_id)
{
    const auto list_index = find_project_position(project_id);
    if (list_index.has_value())
        return get_typed_object<Project>(*list_index);

    return {};
}

void ProjectHierarchicalModel::remove_project(const Project &project)
{
    const auto list_index = find_project_position(project);
    if (list_index.has_value()) {
        const auto hierarchy_it = hierarchy.find(project);
        if (hierarchy_it != hierarchy.cend())
            hierarchy.erase(hierarchy_it);

        remove(*list_index);
    }
}

void ProjectHierarchicalModel::remove_project(const std::size_t project_id)
{
    const auto list_index = find_project_position(project_id);
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

std::optional<guint> ProjectHierarchicalModel::find_project_position(const Project &project) const
{
    return find_project_position(project.get_controller_id());
}

std::optional<guint> ProjectHierarchicalModel::find_project_position(const std::size_t project_id) const
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
