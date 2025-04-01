/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/1/25.
//

#ifndef HIERARCHICALPROJECTMODEL_HPP
#define HIERARCHICALPROJECTMODEL_HPP

#include <giomm/liststore.h>

#include "IProjectModel.hpp"
#include "../StorageEqualityFunctor.hpp"
#include "../StorageHashFunctor.hpp"
#include "../Subsystem/SubsystemHierarchicalModel.hpp"

namespace optifol
{

class ProjectHierarchicalModel :
        public IProjectModel,
        public Gio::ListStore<Project>
{
public:
    void register_project(Glib::RefPtr<Project>&& project) override;

    [[nodiscard]] Glib::RefPtr<Project> get_project(const Project& project) override;

    [[nodiscard]] Glib::RefPtr<Project> get_project(std::size_t project_id) override;

    void remove_project(const Project& project) override;

    void remove_project(std::size_t project_id) override;

    [[nodiscard]] Glib::RefPtr<SubsystemHierarchicalModel> expand_project(const Project& project);

    [[nodiscard]] Glib::RefPtr<SubsystemHierarchicalModel> expand_project(std::size_t project_id);

private:
    std::optional<guint> find_project_position(const Project& project) const;

    std::optional<guint> find_project_position(std::size_t project_id) const;

    Glib::RefPtr<Project> dummy_project = Glib::make_refptr_for_instance(new Project(0, {}, {}, {}));

    std::unordered_map<Glib::RefPtr<Project>, Glib::RefPtr<SubsystemHierarchicalModel>, StorageHashFunctor<Project>,
        StorageEqualityFunctor<Project>> hierarchy;
};

}

#endif
