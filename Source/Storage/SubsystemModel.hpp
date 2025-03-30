/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#ifndef SUBSYSTEMMODEL_HPP
#define SUBSYSTEMMODEL_HPP

#include "RequirementModel.hpp"
#include "StorableObjectModelBase.hpp"
#include "StorageEqualityFunctor.hpp"
#include "StorageHashFunctor.hpp"
#include "Subsystem.hpp"

namespace optifol {

class SubsystemModel :
        virtual public StorableObjectModelBase<Subsystem>
{
public:
    void add_requirement_model(const Glib::RefPtr<Subsystem>& owner_subsystem, const Glib::RefPtr<RequirementModel>& model);

    void unregister_requirement_model(const Glib::RefPtr<Subsystem>& owner_subsystem);

    void unregister_requirement_model(std::size_t owner_subsystem_id);

    Glib::RefPtr<RequirementModel> get_requirement_model(const Glib::RefPtr<Subsystem>& owner_subsystem) const;

    Glib::RefPtr<RequirementModel> get_requirement_model(std::size_t owner_subsystem_id) const;

protected:
    SubsystemModel() = default;

    std::unordered_map<Glib::RefPtr<Subsystem>, Glib::RefPtr<RequirementModel>, StorageHashFunctor<Subsystem>,
        StorageEqualityFunctor<Subsystem>> requirement_models;
};

}

#endif
