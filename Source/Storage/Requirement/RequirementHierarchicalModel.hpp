/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the Glib-backed hierarchical-based storable object for requirements
 * @author Oliver Dixon
 * @date 2025-04-05
 * @version Development
 */

#ifndef REQUIREMENTHIERARCHICALMODEL_HPP
#define REQUIREMENTHIERARCHICALMODEL_HPP

#include "Requirement.hpp"
#include "../GlibStorableObjectModelBase.hpp"
#include "../IStorableObjectModel.hpp"

namespace optifol
{

/**
 * @class RequirementHierarchicalModel
 * @brief The requirements model forming part of a hierarchical Glib-backed list model
 */
class RequirementHierarchicalModel :
        public IStorableObjectModel<Requirement>,
        public GlibStorableObjectModelBase<Requirement>
{
public:
    explicit RequirementHierarchicalModel(std::size_t relevant_subsystem_tag);

    void register_object(Glib::RefPtr<Requirement>&& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_object(const Requirement& requirement) override;

    [[nodiscard]] Glib::RefPtr<Requirement> get_object(std::size_t requirement_id) override;

    void remove_object(const Requirement& requirement) override;

    void remove_object(std::size_t requirement_id) override;

private:
    std::optional<guint> find_object_position(const Requirement& requirement) const override;

    std::optional<guint> find_object_position(std::size_t requirement_id) const override;

    Glib::RefPtr<Requirement> dummy_requirement = Glib::make_refptr_for_instance(new Requirement(0, 0, {}, {}, {}, {},
        0, {}, 0, 0));

    const std::size_t relevant_subsystem_tag;
};

}

#endif
