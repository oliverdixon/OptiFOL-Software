/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the PostgreSQL storable object model base
 * @author Oliver Dixon
 * @date 2025-03-20
 * @version Development
 */

#ifndef PGSTORABLEOBJECTMODELBASE_HPP
#define PGSTORABLEOBJECTMODELBASE_HPP

#include <queue>
#include <variant>
#include <pqxx/row>

#include "ICacheableStorableObjectModel.hpp"

namespace optifol
{

class PGStorableObjectModelBase :
        public ICacheableStorableObjectModel
{
public:
    /**
     * @brief Enqueue an object, identified by its numerical ID, to be loaded into the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_load(pqxx::row&& row);

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be reloaded into the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_reload(pqxx::row&& row);

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be unloaded from the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_unload(pqxx::row&& row);

    void enqueue_load(std::size_t id) override;

    void enqueue_reload(std::size_t id) override;

    void enqueue_unload(std::size_t id) override;

    void flush_inbound_insert() override;

    void flush_inbound_update() override;

    void flush_inbound_delete() override;

    void flush_outbound_insert() override;

    void flush_outbound_update() override;

    void flush_outbound_delete() override;

protected:
    explicit PGStorableObjectModelBase(pqxx::connection& connection);

    [[nodiscard]] virtual pqxx::result filter_objects(const std::ostringstream& sql_parameter,
        std::size_t maximum_return_count) const = 0;

    virtual void emplace_object(const pqxx::row& row) = 0;

    virtual void deplace_object(std::size_t id) = 0;

    std::queue<std::variant<std::size_t, pqxx::row>> load_queue;
    std::queue<std::variant<std::size_t, pqxx::row>> reload_queue;
    std::queue<std::variant<std::size_t, pqxx::row>> unload_queue;

    pqxx::connection& connection;
};

}

#endif
