/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/1/25.
//

#include "SubsystemHierarchicalModel.hpp"

namespace optifol
{

void SubsystemHierarchicalModel::register_subsystem(Glib::RefPtr<Subsystem> &&subsystem)
{
    requirement_models.emplace(subsystem, Glib::make_refptr_for_instance(new RequirementHierarchicalModel()));
    append(subsystem);
}

Glib::RefPtr<Subsystem> SubsystemHierarchicalModel::get_subsystem(const Subsystem &subsystem)
{
    const auto list_index = find_subsystem_position(subsystem);
    if (list_index.has_value())
        return get_typed_object<Subsystem>(*list_index);

    return {};
}

Glib::RefPtr<Subsystem> SubsystemHierarchicalModel::get_subsystem(const std::size_t subsystem_id)
{
    const auto list_index = find_subsystem_position(subsystem_id);
    if (list_index.has_value())
        return get_typed_object<Subsystem>(*list_index);

    return {};
}

void SubsystemHierarchicalModel::remove_subsystem(const Subsystem &subsystem)
{
    const auto list_index = find_subsystem_position(subsystem);
    if (list_index.has_value()) {
        const auto model_it = requirement_models.find(subsystem);
        if (model_it != requirement_models.cend())
            requirement_models.erase(model_it);

        remove(*list_index);
    }
}

void SubsystemHierarchicalModel::remove_subsystem(const std::size_t subsystem_id)
{
    const auto list_index = find_subsystem_position(subsystem_id);
    if (list_index.has_value()) {
        const auto model_it = requirement_models.find(subsystem_id);
        if (model_it != requirement_models.cend())
            requirement_models.erase(model_it);

        remove(*list_index);
    }
}

Glib::RefPtr<RequirementHierarchicalModel> SubsystemHierarchicalModel::query_subsystem(const Subsystem &subsystem)
{
    const auto model_it = requirement_models.find(subsystem);
    if (model_it == requirement_models.cend())
        return {};

    return model_it->second;
}

Glib::RefPtr<RequirementHierarchicalModel> SubsystemHierarchicalModel::query_subsystem(const std::size_t subsystem_id)
{
    const auto model_it = requirement_models.find(subsystem_id);
    if (model_it == requirement_models.cend())
        return {};

    return model_it->second;
}

std::optional<guint> SubsystemHierarchicalModel::find_subsystem_position(const Subsystem &subsystem) const
{
    return find_subsystem_position(subsystem.get_controller_id());
}

std::optional<guint> SubsystemHierarchicalModel::find_subsystem_position(const std::size_t subsystem_id) const
{
    auto [found, position] = find(dummy_subsystem,
        [subsystem_id](const Glib::RefPtr<const Subsystem> &candidate,
                const Glib::RefPtr<const Subsystem> &dummy) -> auto
        {
            std::ignore = dummy;
            return candidate->get_controller_id() == subsystem_id;
        }
    );

    if (found)
        return position;

    return {};
}

}
