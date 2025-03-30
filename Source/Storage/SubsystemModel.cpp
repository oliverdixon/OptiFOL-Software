/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#include "SubsystemModel.hpp"

namespace optifol
{

void SubsystemModel::add_requirement_model(const Glib::RefPtr<Subsystem> &owner_subsystem,
    const Glib::RefPtr<RequirementModel> &model)
{
    requirement_models.emplace(owner_subsystem, model);
}

void SubsystemModel::unregister_requirement_model(const Glib::RefPtr<Subsystem> &owner_subsystem)
{
    requirement_models.erase(owner_subsystem);
}

void SubsystemModel::unregister_requirement_model(const std::size_t owner_subsystem_id)
{
    const auto entry_it = requirement_models.find(owner_subsystem_id);
    requirement_models.erase(entry_it);
}

Glib::RefPtr<RequirementModel> SubsystemModel::get_requirement_model(
    const Glib::RefPtr<Subsystem> &owner_subsystem) const
{
    const auto &ss_model_it = requirement_models.find(owner_subsystem);
    return ss_model_it == requirement_models.cend() ? nullptr : ss_model_it->second;
}

Glib::RefPtr<RequirementModel> SubsystemModel::get_requirement_model(const std::size_t owner_subsystem_id) const
{
    const auto &ss_model_it = requirement_models.find(owner_subsystem_id);
    return ss_model_it == requirement_models.cend() ? nullptr : ss_model_it->second;
}

}
