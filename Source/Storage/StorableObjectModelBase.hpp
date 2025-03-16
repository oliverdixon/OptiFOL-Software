/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the GTK-compatible storage model base for OptiFOL storable objects
 * @author Oliver Dixon
 * @date 2025-03-16
 * @version Development
 */

#ifndef STORABLEOBJECTMODELBASE_HPP
#define STORABLEOBJECTMODELBASE_HPP

#include <queue>
#include <giomm/liststore.h>

#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class StorableObjectModelBase
 * @brief A GTK model to manage StorableType objects with simple caching and propagation support
 * @details This base class provides the top level of the model hierarchy for storing OptiFOL objects compliant with the
 *  StorableType concept. In particular, it is the most general templated base extending the Gio::ListStore model. The
 *  typical immediate inheritor of this base is a StorableType-templated class for a specific backend, e.g. PostgreSQL
 *  as with PGStorableObjectModel. The backend-specific class should then be inherited further for each individual
 *  StorableType, which is fully specialised to the backend, and the StorableType; see PGProjectModel for example.
 * @tparam Type The concrete StorableType cached by the base container
 */
template<StorableType Type>
class StorableObjectModelBase :
        public Gio::ListStore<Type>
{
public:
    ~StorableObjectModelBase() override = default;

    /**
     * @brief Flush any pending loads queued by the instance
     */
    virtual void load() = 0;

    /**
     * @brief Flush any pending reloads queued by the instance
     */
    virtual void reload() = 0;

    /**
     * @brief Flush any pending unloads queued by the instance
     */
    virtual void unload() = 0;

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be loaded into the cache instance
     * @param id The ID of the object to enqueue
     */
    void enqueue_load(const std::size_t id)
    {
        load_queue.push(id);
    }

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be reloaded into the cache instance
     * @param id The ID of the object to enqueue
     */
    void enqueue_reload(const std::size_t id)
    {
        reload_queue.push(id);
    }

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be unloaded from the cache instance
     * @param id The ID of the object to enqueue
     */
    void enqueue_unload(const std::size_t id)
    {
        unload_queue.push(id);
    }

protected:
    std::queue<std::size_t> load_queue;
    std::queue<std::size_t> reload_queue;
    std::queue<std::size_t> unload_queue;
};

}

#endif
