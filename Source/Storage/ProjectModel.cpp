/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#include "ProjectModel.hpp"

namespace optifol
{

void ProjectModel::add_subsystem_model(const Glib::RefPtr<Project> &owner_project,
        const Glib::RefPtr<SubsystemModel> &model)
{
    subsystem_models.emplace(owner_project, model);
}

void ProjectModel::unregister_subsystem_model(const Glib::RefPtr<Project> &owner_project)
{
    subsystem_models.erase(owner_project);
}

void ProjectModel::unregister_subsystem_model(std::size_t owner_project_id)
{
    /*
     * Cumbersome, but std::unordered_map::erase doesn't support transparent lookup. (Why not?) Anyway, remove the
     * subsystem slot thus calling the destructing of the mapped value (subsystem model), then remove the project itself
     * from ourselves as the project model.
     */
    const auto entry_it = subsystem_models.find(owner_project_id);
    subsystem_models.erase(entry_it);
}

Glib::RefPtr<SubsystemModel> ProjectModel::get_subsystem_model(const Glib::RefPtr<Project> &owner_project) const
{
    const auto& ss_model_it = subsystem_models.find(owner_project);
    return ss_model_it == subsystem_models.cend() ? nullptr : ss_model_it->second;
}

Glib::RefPtr<SubsystemModel> ProjectModel::get_subsystem_model(const std::size_t owner_project_id) const
{
    const auto& ss_model_it = subsystem_models.find(owner_project_id);
    return ss_model_it == subsystem_models.cend() ? nullptr : ss_model_it->second;
}

}
