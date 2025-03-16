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

#include <pqxx/connection>

#include "StorableObjectModelBase.hpp"
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
        public StorableObjectModelBase<Type>
{
public:
    ~PGStorableObjectModel() override = default;

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
