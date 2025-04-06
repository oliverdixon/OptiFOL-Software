/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the Glib-backed hierarchical-based storable object for subsystems
 * @author Oliver Dixon
 * @date 2025-04-05
 * @version Development
 */

#ifndef SUBSYSTEMHIERARCHICALMODEL_HPP
#define SUBSYSTEMHIERARCHICALMODEL_HPP

#include "Subsystem.hpp"
#include "../GlibStorableObjectModelBase.hpp"
#include "../IStorableObjectModel.hpp"
#include "../StorageEqualityFunctor.hpp"
#include "../StorageHashFunctor.hpp"
#include "../Requirement/RequirementHierarchicalModel.hpp"

namespace optifol
{

/**
 * @class SubsystemHierarchicalModel
 * @brief The subsystems model forming part of a hierarchical Glib-backed list model
 */
class SubsystemHierarchicalModel :
        public IStorableObjectModel<Subsystem>,
        public GlibStorableObjectModelBase<Subsystem>
{
public:
    explicit SubsystemHierarchicalModel(std::size_t relevant_project_tag);

    void register_object(Glib::RefPtr<Subsystem>&& subsystem) override;

    [[nodiscard]] Glib::RefPtr<Subsystem> get_object(const Subsystem& subsystem) override;

    [[nodiscard]] Glib::RefPtr<Subsystem> get_object(std::size_t subsystem_id) override;

    void remove_object(std::size_t subsystem_id) override;

    /**
     * @brief Query the model for a particular subsystem, and get the associated requirements model
     * @param subsystem A full instantiation of the target subsystem
     * @return The requirements model associated with the target subsystem, or an empty ref-counted container if there
     *  was no suitable subsystem loaded into the model
     */
    [[nodiscard]] Glib::RefPtr<RequirementHierarchicalModel> query_object(const Subsystem& subsystem);

    /**
     * @brief Query the model for a particular subsystem, and get the associated requirements model
     * @param subsystem_id The ID of the target subsystem
     * @return The requirements model associated with the target subsystem, or an empty ref-counted container if there
     *  was no suitable subsystem loaded into the model
     */
    [[nodiscard]] Glib::RefPtr<RequirementHierarchicalModel> query_object(std::size_t subsystem_id);

private:
    std::optional<guint> find_object_position(const Subsystem& subsystem) const override;

    std::optional<guint> find_object_position(std::size_t subsystem_id) const override;

    Glib::RefPtr<Subsystem> dummy_subsystem = Glib::make_refptr_for_instance(new Subsystem(0, 0, {}, {}, {}));

    std::unordered_map<Glib::RefPtr<Subsystem>, Glib::RefPtr<RequirementHierarchicalModel>,
        StorageHashFunctor<Subsystem>, StorageEqualityFunctor<Subsystem>> requirement_models;

    const std::size_t relevant_project_tag;
};

}

#endif
