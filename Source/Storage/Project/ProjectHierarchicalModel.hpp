/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the Glib-backed hierarchical-based storable object for projects
 * @author Oliver Dixon
 * @date 2025-04-05
 * @version Development
 */

#ifndef PROJECTHIERARCHICALMODEL_HPP
#define PROJECTHIERARCHICALMODEL_HPP

#include "Project.hpp"
#include "../GlibStorableObjectModelBase.hpp"
#include "../IStorableObjectModel.hpp"
#include "../StorageEqualityFunctor.hpp"
#include "../StorageHashFunctor.hpp"
#include "../Subsystem/SubsystemHierarchicalModel.hpp"

namespace optifol
{

/**
 * @class ProjectHierarchicalModel
 * @brief The Project object model forming part of a hierarchical Glib-backed list model
 */
class ProjectHierarchicalModel :
        public IStorableObjectModel<Project>,
        public GlibStorableObjectModelBase<Project>
{
public:
    void register_object(Glib::RefPtr<Project>&& project) override;

    [[nodiscard]] Glib::RefPtr<Project> get_object(const Project& project) override;

    [[nodiscard]] Glib::RefPtr<Project> get_object(std::size_t project_id) override;

    void remove_object(std::size_t project_id) override;

    /**
     * @brief Query the model for a particular project, and get the associated subsystems model
     * @param project A full instantiation of the target project
     * @return The subsystems model associated with the target project, or an empty ref-counted container if there
     *  was no suitable project loaded into the model
     */
    [[nodiscard]] Glib::RefPtr<SubsystemHierarchicalModel> expand_project(const Project& project);

    /**
     * @brief Query the model for a particular project, and get the associated subsystems model
     * @param project_id The ID of the target project
     * @return The subsystems model associated with the target project, or an empty ref-counted container if there
     *  was no suitable project loaded into the model
     */
    [[nodiscard]] Glib::RefPtr<SubsystemHierarchicalModel> expand_project(std::size_t project_id);

private:
    std::optional<guint> find_object_position(const Project& project) const override;

    std::optional<guint> find_object_position(std::size_t project_id) const override;

    Glib::RefPtr<Project> dummy_project = Glib::make_refptr_for_instance(new Project(0, {}, {}, {}));

    std::unordered_map<Glib::RefPtr<Project>, Glib::RefPtr<SubsystemHierarchicalModel>, StorageHashFunctor<Project>,
        StorageEqualityFunctor<Project>> hierarchy;
};

}

#endif
