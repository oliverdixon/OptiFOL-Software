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

#include <glibmm/object.h>

#include <chrono>

namespace optifol
{

/**
 * @class IStorageObject
 * @brief Defines the interface for a storable object in the Optifol object hierarchy
 */
class IStorageObject :
        public Glib::Object
{
public:
    /**
     * @typedef TimeT
     * @brief The internal C++ type used to manipulate times on the system clock
     */
    using TimeT = std::chrono::system_clock::time_point;

    /**
     * @brief Destruct the storage object
     */
    ~IStorageObject() override = default;

    /**
     * @brief Gets the human-readable string identifier of the object
     * @return The object human-readable string identifier
     */
    [[nodiscard]] virtual std::string get_identifier() const = 0;

    /**
     * @brief Gets the time at which the object was created
     * @return The object creation time
     */
    [[nodiscard]] virtual TimeT get_creation_time() const = 0;

    /**
     * @brief Gets the time at which the object was most recently mutated and hence required its place in the storage
     *  model to be invalidated by the StorageController.
     * @return The object last-modified time
     */
    [[nodiscard]] virtual TimeT get_modified_time() const = 0;

    /**
     * @brief Set the human-readable identifier of the storable object
     * @param name_candidate The new string identifier
     * @note This member function updates the last-modified time to the current time
     */
    virtual void set_identifier(const std::string& name_candidate) = 0;

    /**
     * @brief Set the creation time of the storable object
     * @param time_candidate The new creation time
     * @note This member function updates the last-modified time to the current time
     * @warning Invoking this function will produce a Log4cxx warning record due to the supposed immutability of the
     *  creation time. We provide a mutator here for flexibility with all storage backends (e.g. external databases).
     */
    virtual void set_creation_time(const TimeT& time_candidate) = 0;

    /**
     * @brief Set the last-modified time of the storable object
     * @param time_candidate The new last-modified time
     * @note This member function does not update the last-modified time of the storable object
     */
    virtual void set_modified_time(const TimeT& time_candidate) = 0;
};


// Current Clang 18 bug reports Doxygen violations for uses of @tparam on templated concepts.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
/**
 * @concept StorableType
 * @brief Represents a type that is declared to be a permanently storable object in the Optifol type system
 * @tparam Type The implementing type of the storable object
 */
template<typename Type>
concept StorableType = std::derived_from<Type, IStorageObject>;
#pragma clang diagnostic pop

}

#endif
