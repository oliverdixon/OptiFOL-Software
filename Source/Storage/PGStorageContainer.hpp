/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class template specification of the PostgreSQL storage container
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#ifndef PGSTORAGECONTAINER_HPP
#define PGSTORAGECONTAINER_HPP

#include <unordered_set>

#include "IStorageObject.hpp"
#include "PGNotificationReceiver.hpp"

namespace optifol
{

/**
 * @class PGStorageContainer
 * @brief A PGStorageContainer is a PG-specific convenient container within which a class of stored objects,
 *  identifiable by their C++ type, may be maintained. Caches and receivers are also transparently maintained, but not
 *  managed.
 * @see PGNotificationReceiver
 * @tparam ConcreteType The concrete storable object type maintained by the container
 */
template<class ConcreteType>
    requires std::derived_from<ConcreteType, IStorageObject>
struct PGStorageContainer
{
    /**
     * @brief Create a new storage container for the templated type, complete with PG notification Rx's for actions
     * @param connection The PG DB connection against which the notification receivers should be registered
     * @param entity_name The human-readable name of the contained type
     */
    PGStorageContainer(pqxx::connection &connection, const std::string &entity_name):
        insert_rx(connection, entity_name, PGNotificationReceiver::Action::Insert),
        update_rx(connection, entity_name, PGNotificationReceiver::Action::Update),
        delete_rx(connection, entity_name, PGNotificationReceiver::Action::Delete)
    {
    }

    const PGNotificationReceiver insert_rx;
    const PGNotificationReceiver update_rx;
    const PGNotificationReceiver delete_rx;

    std::unordered_set<std::unique_ptr<ConcreteType>> cache;
    static constexpr std::size_t cachable_elements = 100; // TODO testing only
};

}

#endif
