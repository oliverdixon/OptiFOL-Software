/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef ICACHEABLESTORABLEOBJECTMODEL_HPP
#define ICACHEABLESTORABLEOBJECTMODEL_HPP

namespace optifol
{

class ICacheableStorableObjectModel
{
public:
    virtual ~ICacheableStorableObjectModel() = default;

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be loaded into the cache instance
     * @param id The ID of the object to enqueue
     */
    virtual void enqueue_load(std::size_t id) = 0;

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be reloaded into the cache instance
     * @param id The ID of the object to enqueue
     */
    virtual void enqueue_reload(std::size_t id) = 0;

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be unloaded from the cache instance
     * @param id The ID of the object to enqueue
     */
    virtual void enqueue_unload(std::size_t id) = 0;

    /**
     * @brief Propagate any loads from the database as requested by the instance
     */
    virtual void flush_inbound_insert() = 0;

    /**
     * @brief Propagate any reloads from the database as requested by the instance
     */
    virtual void flush_inbound_update() = 0;

    /**
     * @brief Propagate any unloads from the database as requested by the instance
     */
    virtual void flush_inbound_delete() = 0;

    virtual void flush_outbound_insert() = 0;

    virtual void flush_outbound_update() = 0;

    virtual void flush_outbound_delete() = 0;

    /**
     * @brief Get the number of storable objects in the model
     * @return The number of objects in the model
     */
    [[nodiscard]] virtual std::size_t get_item_count() const noexcept = 0;
};

}

#endif
