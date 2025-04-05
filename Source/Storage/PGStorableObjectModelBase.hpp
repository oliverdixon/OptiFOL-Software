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
#include <unordered_set>
#include <variant>
#include <pqxx/row>
#include <sigc++/signal.h>

#include "ICacheableStorableObjectModel.hpp"
#include "StorageEqualityFunctor.hpp"
#include "StorageHashFunctor.hpp"

namespace optifol
{

template<StorableType Type>
class PGStorableObjectModelBase :
        public ICacheableStorableObjectModel<Type>
{
public:
    /**
     * @brief Enqueue an object, identified by its numerical ID, to be loaded into the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_load(pqxx::row&& row)
    {
        load_queue.emplace(row);
    }

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be reloaded into the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_reload(pqxx::row&& row)
    {
        reload_queue.emplace(row);
    }

    /**
     * @brief Enqueue an object, identified by its numerical ID, to be unloaded from the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_unload(pqxx::row&& row)
    {
        unload_queue.emplace(row);
    }

    void enqueue_load(std::size_t id) override
    {
        load_queue.emplace(id);
    }

    void enqueue_reload(std::size_t id) override
    {
        reload_queue.emplace(id);
    }

    void enqueue_unload(std::size_t id) override
    {
        unload_queue.emplace(id);
    }

    void flush_inbound_insert() override
    {
        if (load_queue.empty())
            return;

        // Build the queued IDs into a stream for substitution into the SQL query
        std::ostringstream sql_parameter;
        std::size_t sql_parameter_count = 0;

        sql_parameter << '{';

        while (!load_queue.empty()) {
            /*
             * If we have a raw ID, build it into the stream to be fetched from the DB. If we have a prefetched row, emplace
             * it immediately.
             */

            std::visit([this, &sql_parameter, &sql_parameter_count]<typename DeducedType>(DeducedType &&arg)
            {
                using DecayedType = std::decay_t<DeducedType>;
                if constexpr (std::is_same_v<DecayedType, std::size_t>) {
                    sql_parameter << arg << ',';
                    ++sql_parameter_count;
                } else if constexpr (std::is_same_v<DecayedType, pqxx::row>)
                    emplace_object(arg);
            }, load_queue.front());

            load_queue.pop();
        }

        if (sql_parameter_count > 0) {
            // Remove the trailing delimiter from the stream
            sql_parameter.seekp(-1, std::ios_base::end);
            sql_parameter << '}';

            // Run the query to filter the queued IDs from the DB object table, and load into the cache
            const auto db_result = filter_objects(sql_parameter, 128);
            for (const auto &row: db_result)
                emplace_object(row);
        }
    }

    void flush_inbound_update() override
    {
        throw std::runtime_error("Unimplemented"); // TODO
    }

    void flush_inbound_delete() override
    {
        while (!unload_queue.empty()) {
            std::size_t id;

            std::visit([&id]<typename DeducedType>(DeducedType &&arg)
            {
                /*
                 * If we have a raw ID, the identity function will suffice to assign an ID to target for deletion. If we
                 * have a full row (unlikely for unloading, but still possible on the API), grab the ID from the row.
                 */

                using DecayedType = std::decay_t<DeducedType>;
                if constexpr (std::is_same_v<DecayedType, std::size_t>)
                    id = arg;
                else if constexpr (std::is_same_v<DecayedType, pqxx::row>)
                    id = arg[0].template as<std::size_t>();
            }, unload_queue.front());

            unload_queue.pop();
            deplace_object(id);
        }
    }

    void flush_outbound_insert() override
    {
        throw std::runtime_error("Unimplemented"); // TODO
    }

    void flush_outbound_update() override
    {
        throw std::runtime_error("Unimplemented"); // TODO
    }

    void flush_outbound_delete() override
    {
        throw std::runtime_error("Unimplemented"); // TODO
    }

    void add_insert_subscriber(
        sigc::slot<typename ICacheableStorableObjectModel<Type>::InsertionCallbackSignature>&& slot,
        const bool onboard = true) override
    {
        auto& signal = insert_signals.emplace_back();
        signal.connect(std::move(slot));

        if (onboard)
            for (const auto& item : model_contents)
                inform_insertion(item);
    }

protected:
    explicit PGStorableObjectModelBase(pqxx::connection& connection) :
            connection(connection)
    { }

    [[nodiscard]] virtual pqxx::result filter_objects(const std::ostringstream& sql_parameter,
        std::size_t maximum_return_count) const = 0;

    virtual void emplace_object(const pqxx::row& row) = 0;

    virtual void deplace_object(std::size_t id) = 0;

    void inform_insertion(Glib::RefPtr<Type> inserted_item) const
    {
        for (const auto& signal : insert_signals)
            signal(std::move(inserted_item));
    }

    std::queue<std::variant<std::size_t, pqxx::row>> load_queue;
    std::queue<std::variant<std::size_t, pqxx::row>> reload_queue;
    std::queue<std::variant<std::size_t, pqxx::row>> unload_queue;

    pqxx::connection& connection;

    std::unordered_set<Glib::RefPtr<Type>, StorageHashFunctor<Type>, StorageEqualityFunctor<Type>> model_contents;

private:
    std::vector<sigc::signal<typename ICacheableStorableObjectModel<Type>::InsertionCallbackSignature>> insert_signals;
};

}

#endif
