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

#include "ICacheableStorableObjectModel.hpp"
#include "ModelPublisherBase.hpp"
#include "StorageEqualityFunctor.hpp"
#include "StorageHashFunctor.hpp"

namespace optifol
{

/**
 * @class PGStorableObjectModelBase
 * @brief The PostgreSQL-specialised implementation base for the ICacheableStorageObjectModel.
 * @details This class provides implementations for the suite of caching, propagation, and subscription functions,
 *  specialised to the PostgreSQL database backend. For this model implementation, fast lookup and insertion is
 *  prioritised over iteration, hence an unordered set is used as the STL container. Derived classes may override any
 *  virtuals, as required for each particular template specialisation on StorableType. Deriving classes must always
 *  implement emplacement and deplacement logic for constructing/registering and destructing/de-registering items,
 *  respectively. They're also required to provide a DB-native filtering operation based on some given SQL parameter
 *  string. 
 * @tparam Type The concrete type of the objects to be stored
 */
template<StorableType Type>
class PGStorableObjectModelBase :
        public ICacheableStorableObjectModel<Type>,
        public ModelPublisherBase<Type>
{
public:
    Glib::RefPtr<Type>* begin() const
    {
        return model_contents.begin();
    }

    Glib::RefPtr<Type>* end() const
    {
        return model_contents.end();
    }

    Glib::RefPtr<Type>* cbegin() const
    {
        return model_contents.cbegin();
    }

    Glib::RefPtr<Type>* cend() const
    {
        return model_contents.cend();
    }

    /**
     * @brief Enqueue an object, already loaded from the DB, to be loaded into the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_load(pqxx::row&& row)
    {
        load_queue.emplace(row);
    }

    /**
     * @brief Enqueue an object, already loaded from the DB, to be reloaded into the cache instance
     * @param row The complete row of the object to enqueue
     */
    void enqueue_reload(pqxx::row&& row)
    {
        reload_queue.emplace(row);
    }

    /**
     * @brief Enqueue an object, already loaded from the DB, to be unloaded from the cache instance
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
             * If we have a raw ID, build it into the stream to be fetched from the DB. If we have a prefetched row,
             * emplace it immediately.
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
        // TODO
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
        // TODO
    }

    void flush_outbound_update() override
    {
        // TODO
    }

    void flush_outbound_delete() override
    {
        // TODO
    }

    /**
     * @copybrief ModelPublisherBase<Type>::add_insert_subscriber
     * @param slot The callback exposed by the subscriber's API
     * @param onboard Should the instance immediately send signals to the new subscriber for each model item?
     * @return The newly added signal
     */
    const sigc::signal<typename ModelPublisherBase<Type>::InsertionCallbackSignature>&
        add_insert_subscriber(
            sigc::slot<typename ModelPublisherBase<Type>::InsertionCallbackSignature>&& slot,
            const bool onboard) override
    {
        auto& signal = ModelPublisherBase<Type>::add_insert_subscriber(
            std::forward<decltype(slot)>(slot), onboard);

        if (onboard)
            for (auto item : model_contents)
                /*
                 * The signal needs an r-value. The iterator provides constant references, so a temporary copy is made,
                 * scoped to the loop, and immediately converted to an r-value.
                 */
                signal(std::move(item));

        return signal;
    }

protected:
    /**
     * @brief Construct the PostgreSQL storable object model with a pre-connected PG DB instance
     * @param connection The active database connection reference
     * @warning It is implicitly assumed that any model bases will be encapsulated within the scope of the corresponding
     *  database controller, which will maintain mutable references to the active database connection. If the connection
     *  reference becomes unscoped and hence destructed, behaviour is undefined. Also, if the connection drops out of
     *  the Connected state, the behaviour is not undefined but member functions of models are likely to throw
     *  PG-specific exceptions; it is the responsibility of the database controller to ensure that these specialised
     *  exception types are not passed directly to the storage-agnostic handler.
     */
    explicit PGStorableObjectModelBase(pqxx::connection& connection) :
            connection(connection)
    { }

    /**
     * @brief Query and collect the results of a read SELECT..WHERE-like statement on the database, filtered according
     *  to primary keys enumerated by the given parameter vector.
     * @param sql_parameter The streamed SQL parameter to splice into the filtering query, typically an unordered
     *  comma-separated (SQL-syntax-compliant) list of record IDs to retrieve from the DB.
     * @param maximum_return_count The maximum number of records to return
     * @return The records received from the database query
     * @warning The SQL parameter query is trusted and directly passed to the database; it is not sanitised nor used as
     *  part of a prepared statement. Thus, this member function is liable to injection and should not be invoked
     *  directly on the result of untrusted (e.g. user-sourced) input.
     * @throws pqxx::failure The failure of the PQXX backend while executing the query
     */
    [[nodiscard]] virtual pqxx::result filter_objects(const std::ostringstream& sql_parameter,
        std::size_t maximum_return_count) const = 0;

    /**
     * @brief Construct an object described by the given PG DB row and store it in the model
     * @param row The row retrieved from the DB, which describes the item to be constructed and appended.
     */
    virtual void emplace_object(const pqxx::row& row) = 0;

    /**
     * @brief Remove (and potentially destruct) the item with the given identifier from the model
     * @param id The ID of the item to be removed from the model
     */
    virtual void deplace_object(std::size_t id) = 0;

    std::queue<std::variant<std::size_t, pqxx::row>> load_queue;
    std::queue<std::variant<std::size_t, pqxx::row>> reload_queue;
    std::queue<std::variant<std::size_t, pqxx::row>> unload_queue;

    pqxx::connection& connection;

    std::unordered_set<Glib::RefPtr<Type>, StorageHashFunctor<Type>, StorageEqualityFunctor<Type>> model_contents;
};

}

#endif
