/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PGPROJECTCACHE_HPP
#define PGPROJECTCACHE_HPP

#include "PGStorableObjectCacheBase.hpp"
#include "Project.hpp"

namespace optifol
{

class PGProjectCache :
        public PGStorableObjectCacheBase<Project>
{
public:
    explicit PGProjectCache(pqxx::connection& connection);

    void load() override;
};

}

#endif
