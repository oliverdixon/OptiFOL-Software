/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/1/25.
//

#ifndef SUBSYSTEMHIERARCHICALMODEL_HPP
#define SUBSYSTEMHIERARCHICALMODEL_HPP

#include <giomm/liststore.h>

#include "ISubsystemModel.hpp"
#include "../StorageEqualityFunctor.hpp"
#include "../StorageHashFunctor.hpp"
#include "../Requirement/RequirementHierarchicalModel.hpp"

namespace optifol
{

class SubsystemHierarchicalModel :
        public ISubsystemModel,
        public Gio::ListStore<Subsystem>
{
public:
    void register_subsystem(Glib::RefPtr<Subsystem>&& subsystem) override;

    [[nodiscard]] Glib::RefPtr<Subsystem> get_subsystem(const Subsystem& subsystem) override;

    [[nodiscard]] Glib::RefPtr<Subsystem> get_subsystem(std::size_t subsystem_id) override;

    void remove_subsystem(const Subsystem& subsystem) override;

    void remove_subsystem(std::size_t subsystem_id) override;

    [[nodiscard]] Glib::RefPtr<RequirementHierarchicalModel> query_subsystem(const Subsystem& subsystem);

    [[nodiscard]] Glib::RefPtr<RequirementHierarchicalModel> query_subsystem(std::size_t subsystem_id);

private:
    std::optional<guint> find_subsystem_position(const Subsystem& subsystem) const;

    std::optional<guint> find_subsystem_position(std::size_t subsystem_id) const;

    Glib::RefPtr<Subsystem> dummy_subsystem = Glib::make_refptr_for_instance(new Subsystem(0, {}, {}, {}));

    std::unordered_map<Glib::RefPtr<Subsystem>, Glib::RefPtr<RequirementHierarchicalModel>,
        StorageHashFunctor<Subsystem>, StorageEqualityFunctor<Subsystem>> requirement_models;
};

}

#endif
