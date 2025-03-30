/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#ifndef PROJECTMODEL_HPP
#define PROJECTMODEL_HPP

#include "Project.hpp"
#include "StorableObjectModelBase.hpp"
#include "StorageEqualityFunctor.hpp"
#include "StorageHashFunctor.hpp"
#include "SubsystemModel.hpp"

namespace optifol
{

class ProjectModel :
        virtual public StorableObjectModelBase<Project>
{
public:
    void add_subsystem_model(const Glib::RefPtr<Project>& owner_project, const Glib::RefPtr<SubsystemModel>& model);

    void unregister_subsystem_model(const Glib::RefPtr<Project>& owner_project);

    void unregister_subsystem_model(std::size_t owner_project_id);

    Glib::RefPtr<SubsystemModel> get_subsystem_model(const Glib::RefPtr<Project>& owner_project) const;

    Glib::RefPtr<SubsystemModel> get_subsystem_model(std::size_t owner_project_id) const;

protected:
    ProjectModel() = default;

    std::unordered_map<Glib::RefPtr<Project>, Glib::RefPtr<SubsystemModel>, StorageHashFunctor<Project>,
        StorageEqualityFunctor<Project>> subsystem_models;
};

}

#endif
