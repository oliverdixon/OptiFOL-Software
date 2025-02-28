/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef PGSTORABLEOBJECTCACHEBASE_HPP
#define PGSTORABLEOBJECTCACHEBASE_HPP

#include <queue>
#include <unordered_set>
#include <pqxx/connection>

#include "StorageHashFunctor.hpp"

namespace optifol
{

template<StorableType Type>
class PGStorableObjectCacheBase
{
public:
    virtual ~PGStorableObjectCacheBase() = default;

    virtual void load()
    {
        if (load_queue.empty())
            return;

        throw std::runtime_error("Not implemented.");
    }

    virtual void reload()
    {
        if (reload_queue.empty())
            return;

        throw std::runtime_error("Not implemented.");
    }

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

    void enqueue_load(const std::size_t id)
    {
        load_queue.push(id);
    }

    void enqueue_reload(const std::size_t id)
    {
        reload_queue.push(id);
    }

    void enqueue_unload(const std::size_t id)
    {
        unload_queue.push(id);
    }

protected:
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
