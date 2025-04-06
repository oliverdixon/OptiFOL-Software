/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition for the cacheable storage model interface
 * @author Oliver Dixon
 * @date 2025-04-05
 * @version Development
 */

#ifndef ICACHEABLESTORABLEOBJECTMODEL_HPP
#define ICACHEABLESTORABLEOBJECTMODEL_HPP

#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class ICacheableStorableObjectModel
 * @brief The basis for cacheable storage object models, supporting deferred propagation with abstract queue-like
 *  structures
 * @tparam Type The concrete type of the objects to be stored
 */
template<StorableType Type>
class ICacheableStorableObjectModel
{
public:
    /**
     * @brief The virtual destructor for the cacheable storable object base
     */
    virtual ~ICacheableStorableObjectModel() = default;

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be loaded into the cache instance
     * @param id The ID of the object to enqueue
     */
    virtual void enqueue_inbound_load(std::size_t id) = 0;

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be reloaded into the cache instance
     * @param id The ID of the object to enqueue
     */
    virtual void enqueue_inbound_reload(std::size_t id) = 0;

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be unloaded from the cache instance
     * @param id The ID of the object to enqueue
     */
    virtual void enqueue_inbound_unload(std::size_t id) = 0;

    /**
     * @brief Propagate any loads from the permanent storage as requested by the instance
     */
    virtual void flush_inbound_insert() = 0;

    /**
     * @brief Propagate any reloads from the permanent storage as requested by the instance
     */
    virtual void flush_inbound_update() = 0;

    /**
     * @brief Propagate any unloads from the permanent storage as requested by the instance
     */
    virtual void flush_inbound_delete() = 0;

    /**
     * @brief Propagate any loads to the permanent storage as requested by the instance
     */
    virtual void flush_outbound_insert() = 0;

    /**
     * @brief Propagate any reloads to the permanent storage as requested by the instance
     */
    virtual void flush_outbound_update() = 0;

    /**
     * @brief Propagate any unloads to the permanent storage as requested by the instance
     */
    virtual void flush_outbound_delete() = 0;

    /**
     * @brief Flush all outbound operations to the permanent storage
     */
    void flush_outbound()
    {
        flush_outbound_insert();
        flush_outbound_update();
        flush_outbound_delete();
    }

    /**
     * @brief Flush all inbound operations to the permanent storage
     */
    void flush_inbound()
    {
        flush_inbound_insert();
        flush_inbound_update();
        flush_inbound_delete();
    }

    /**
     * @brief Get the number of storable objects in the model
     * @return The number of objects in the model
     */
    [[nodiscard]] virtual std::size_t get_item_count() const noexcept = 0;
};

}

#endif
