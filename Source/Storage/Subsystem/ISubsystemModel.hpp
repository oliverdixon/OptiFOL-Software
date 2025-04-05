/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#ifndef SUBSYSTEMMODEL_HPP
#define SUBSYSTEMMODEL_HPP

#include "Subsystem.hpp"
#include "../IAbstractModel.hpp"

namespace optifol {

class ISubsystemModel :
        public IAbstractModel
{
public:
    virtual void register_subsystem(Glib::RefPtr<Subsystem>&& subsystem) = 0;

    [[nodiscard]] virtual Glib::RefPtr<Subsystem> get_subsystem(const Subsystem& subsystem) = 0;

    [[nodiscard]] virtual Glib::RefPtr<Subsystem> get_subsystem(std::size_t subsystem_id) = 0;

    virtual void remove_subsystem(const Subsystem& subsystem) = 0;

    virtual void remove_subsystem(std::size_t subsystem_id) = 0;
};

}

#endif
