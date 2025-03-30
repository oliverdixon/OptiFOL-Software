/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 3/30/25.
//

#ifndef REQUIREMENTMODEL_HPP
#define REQUIREMENTMODEL_HPP

#include "Requirement.hpp"
#include "StorableObjectModelBase.hpp"

namespace optifol
{

class RequirementModel :
        virtual public StorableObjectModelBase<Requirement>
{
protected:
    RequirementModel() = default;
};

}

#endif
