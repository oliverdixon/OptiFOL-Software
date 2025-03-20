/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the PostgreSQL-backed storable object cache base
 * @author Oliver Dixon
 * @date 2025-02-28
 * @version Development
 */

#ifndef PGSTORABLEOBJECTMODEL_HPP
#define PGSTORABLEOBJECTMODEL_HPP

#include "StorableObjectModelBase.hpp"
#include "PGStorableObjectModelBase.hpp"
#include "StorageHashFunctor.hpp"

namespace optifol
{

/**
 * @class PGStorableObjectModel
 * @brief A container to manage StorableType objects under a cache and a PostgreSQL backend
 * @tparam Type The concrete StorableType cached by the base container
 */
template<StorableType Type>
class PGStorableObjectModel :
        public StorableObjectModelBase<Type>,
        public PGStorableObjectModelBase
{
public:
    ~PGStorableObjectModel() override = default;

    void load() final
    {
        PGStorableObjectModelBase::pq_load();
    }

    void reload() final
    {
        PGStorableObjectModelBase::pq_reload();
    }

    void unload() final
    {
        PGStorableObjectModelBase::pq_unload();
    }

protected:
    /**
     * @brief Construct the object cache base
     * @param connection The established PostgreSQL database connection
     */
    explicit PGStorableObjectModel(pqxx::connection& connection):
        connection(connection)
    { }

    pqxx::connection& connection;
};

}

#endif
