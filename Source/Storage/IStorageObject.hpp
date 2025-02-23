/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Interface specification for a storable object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef ISTORAGEOBJECT_HPP
#define ISTORAGEOBJECT_HPP

#include <chrono>

namespace optifol
{

/**
 * @class IStorageObject
 * @brief Defines the interface for a storable object in the OptiFOL object hierarchy
 */
class IStorageObject
{
public:
    using time_t = std::chrono::time_point<std::chrono::utc_clock>;

    /**
     * @brief Destruct the storage object
     * @todo What should be the semantic expression of this d'tor? Is it being removed from the cache, or storage?
     */
    virtual ~IStorageObject() = default;

    /**
     * @brief Gets the human-readable string identifier of the object
     * @return The object human-readable string identifier
     */
    [[nodiscard]] virtual std::string get_identifier() const = 0;

    /**
     * @brief Gets the time at which the object was created
     * @return The object creation time
     */
    [[nodiscard]] virtual time_t get_creation_time() const = 0;

    /**
     * @brief Gets the time at which the object was most recently mutated and hence required its place in the storage
     *  model to be invalidated by the StorageController.
     * @return The object last-modified time
     */
    [[nodiscard]] virtual time_t get_modified_time() const = 0;

    /**
     * @brief Gets the controller-defined internal unique unsigned integer identifier for the object
     * @details For a relational database, this would typically be the primary key. Note that, in the case of an RDB,
     *  the PK is only useful in the context of the object type, as this may, in a similarly implementation-defined
     *  manner, determine the table/entity in which the ID is relevant.
     * @warning The model-controller responsible for the object may decide to hash one or multiple fields to generate an
     *  object identifier. Hence, the risk of non-uniqueness by way of collision is non-zero, and must be assumed to
     *  occur at approximately the same rate as a good hashing algorithm such as SHA256.
     * @return The object unique identifier
     */
    [[nodiscard]] virtual std::size_t get_controller_id() const noexcept = 0;
};

template<typename T>
concept StorableType = requires(T a)
{
    std::derived_from<T, IStorageObject>;
};

}

#endif //ISTORAGEOBJECT_HPP
