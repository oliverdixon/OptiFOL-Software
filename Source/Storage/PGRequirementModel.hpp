/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed requirement storage container
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#ifndef PGREQUIREMENTMODEL_HPP
#define PGREQUIREMENTMODEL_HPP

#include "PGStorableObjectModel.hpp"
#include "Requirement.hpp"

namespace optifol
{

class PGRequirementModel :
        public PGStorableObjectModel<Requirement>
{
public:
    ~PGRequirementModel() override = default;
};

}

#endif
