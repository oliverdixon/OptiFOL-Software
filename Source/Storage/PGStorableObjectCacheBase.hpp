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

#ifndef PGSTORABLEOBJECTCACHEBASE_HPP
#define PGSTORABLEOBJECTCACHEBASE_HPP

#include <queue>
#include <unordered_set>
#include <pqxx/connection>

#include "StorageHashFunctor.hpp"

namespace optifol
{

/**
 * @class PGStorableObjectCacheBase
 * @brief A container to manage StorableType objects under a cache and a PostgreSQL backend
 * @tparam Type The concrete StorableType cached by the base container
 */
template<StorableType Type>
class PGStorableObjectCacheBase
{
public:
    virtual ~PGStorableObjectCacheBase() = default;

    /**
     * @brief Flush any pending loads queued by the instance
     */
    virtual void load()
    {
        if (load_queue.empty())
            return;

        throw std::runtime_error("Not implemented.");
    }

    /**
     * @brief Flush any pending reloads queued by the instance
     */
    virtual void reload()
    {
        if (reload_queue.empty())
            return;

        throw std::runtime_error("Not implemented.");
    }

    /**
     * @brief Flush any pending unloads queued by the instance
     */
    virtual void unload()
    {
        while (!unload_queue.empty()) {
            const auto id = unload_queue.front();
            unload_queue.pop();

            const auto cache_it = cache.find(id);
            if (cache_it != cache.end()) {
                std::cout << "Unloading project " << std::to_string(id) << std::endl;
                cache.erase(cache_it);
            }
        }
    }

    /**
     * @brief Enqueue an object, identified by its PK/ID, to be loaded into the cache instance
     * @param id The PK of the PostgreSQL object to load
     */
    void enqueue_load(const std::size_t id)
    {
        load_queue.push(id);
    }

    /**
     * @brief Enqueue an object, identified by its PK/ID, to be reloaded into the cache instance
     * @param id The PK of the PostgreSQL object to reload
     */
    void enqueue_reload(const std::size_t id)
    {
        reload_queue.push(id);
    }

    /**
     * @brief Enqueue an object, identified by its PK/ID, to be unloaded from the cache instance
     * @param id The PK of the PostgreSQL object to unload
     */
    void enqueue_unload(const std::size_t id)
    {
        unload_queue.push(id);
    }

protected:
    /**
     * @brief Construct the object cache base
     * @param connection The established PostgreSQL database connection
     */
    explicit PGStorableObjectCacheBase(pqxx::connection& connection):
        connection(connection)
    { }

    pqxx::connection& connection;

    std::queue<std::size_t> load_queue;
    std::queue<std::size_t> reload_queue;
    std::queue<std::size_t> unload_queue;

    std::unordered_set<Type, StorageHashFunctor<Type>, std::equal_to<>> cache;
};

}

#endif
