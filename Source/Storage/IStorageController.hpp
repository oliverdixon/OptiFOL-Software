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
     * @brief Destruct the storage controller
     */
    virtual ~IStorageController() = default;

    /**
     * Retrieve a list of all project names from the loaded storage medium
     * @return The list of all project names
     * @deprecated This will be replaced by a method to construct projects by way of a project factory
     */
    [[nodiscard, deprecated]] virtual std::vector<std::string> get_project_names() = 0;
};

}

#endif
