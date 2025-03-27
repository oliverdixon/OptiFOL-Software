/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed unit test storage container
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#ifndef PGUNITTESTMODEL_HPP
#define PGUNITTESTMODEL_HPP

#include "PGStorableObjectModel.hpp"
#include "UnitTest.hpp"

namespace optifol
{

class PGUnitTestModel :
        public PGStorableObjectModel<UnitTest>
{
public:
    ~PGUnitTestModel() override = default;
};

}

#endif
