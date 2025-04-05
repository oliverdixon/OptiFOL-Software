/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#ifndef IREQUIREMENTMODEL_HPP
#define IREQUIREMENTMODEL_HPP

#include "Requirement.hpp"
#include "../IAbstractModel.hpp"

namespace optifol
{

class IRequirementModel :
        public IAbstractModel
{
public:
    virtual void register_requirement(Glib::RefPtr<Requirement>&& requirement) = 0;

    [[nodiscard]] virtual Glib::RefPtr<Requirement> get_requirement(const Requirement& requirement) = 0;

    [[nodiscard]] virtual Glib::RefPtr<Requirement> get_requirement(std::size_t requirement_id) = 0;

    virtual void remove_requirement(const Requirement& requirement) = 0;

    virtual void remove_requirement(std::size_t requirement_id) = 0;
};

}

#endif
