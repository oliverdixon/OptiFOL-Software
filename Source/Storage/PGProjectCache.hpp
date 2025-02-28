/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed project storage container
 * @author Oliver Dixon
 * @date 2025-02-28
 * @version Development
 */

#ifndef PGPROJECTCACHE_HPP
#define PGPROJECTCACHE_HPP

#include "PGStorableObjectCacheBase.hpp"
#include "Project.hpp"

namespace optifol
{

/**
 * @class PGProjectCache
 * @brief The PGStorableObjectCacheBase specialised for project-level stored objects
 */
class PGProjectCache :
        public PGStorableObjectCacheBase<Project>
{
public:
    /**
     * @brief Construct a project cache container
     * @param connection The established PostgreSQL database connection
     */
    explicit PGProjectCache(pqxx::connection& connection);

    void load() override;
};

}

#endif
