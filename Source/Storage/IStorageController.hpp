/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Interface specification for the storage controller
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#ifndef ISTORAGECONTROLLER_HPP
#define ISTORAGECONTROLLER_HPP

#include "PGProjectModel.hpp"

namespace optifol
{

/**
 * @class IStorageController
 * @brief The storage controller provides a uniform interface to the backend engine responsible for persistent storage
 * @warning Constructing a storage controller may perform very expensive operations, such as syncing a file-system,
 *  connecting to a remote database, or mounting a network drive. Use with care!
 */
class IStorageController
{
public:
    /**
     * @brief Destruct the storage controller, closing any held resources and/or external connections.
     * @todo Any pending changes in the caches should be flushed here.
     */
    virtual ~IStorageController() = default;

    /**
     * @brief Force the storage controller to perform an implementation-defined update.
     * @warning The details of this member function are wholly implementation-defined; it may read from a message queue
     *  over a socket and update files appropriately. It should not normally form a polling blocking loop, but this is
     *  similarly implementation-defined.
     */
    virtual void update() = 0;
};

}

#endif
