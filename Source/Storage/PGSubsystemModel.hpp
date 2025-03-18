/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed subsystem storage container
 * @author Oliver Dixon
 * @date 2025-03-18
 * @version Development
 */

#ifndef PGSUBSYSTEMMODEL_HPP
#define PGSUBSYSTEMMODEL_HPP

#include "PGStorableObjectModel.hpp"
#include "Subsystem.hpp"

namespace optifol
{

/**
 * @class PGSubsystemModel
 * @brief The PGStorableObjectModel specialised for subsystem-level stored objects
 */
class PGSubsystemModel :
        public PGStorableObjectModel<Subsystem>
{
public:
    /**
     * @brief Construct a subsystem cache container
     * @param connection The established PostgreSQL database connection
     */
    explicit PGSubsystemModel(pqxx::connection& connection);

    void load() override;

    void reload() override;

    void unload() override;
};

}

#endif
