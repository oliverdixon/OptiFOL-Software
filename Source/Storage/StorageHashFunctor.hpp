/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Template specialisations and class template for the hash functor of a storable object
 * @author Oliver Dixon
 * @date 2025-02-23
 * @version Development
 */

#ifndef STORAGEHASHFUNCTOR_HPP
#define STORAGEHASHFUNCTOR_HPP

#include "IStorageObject.hpp"

// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class std::hash<Type>
 * @brief The standard (non-transparent) hashing functor, templated to a storable type
 * @tparam Type The concrete storable type for which std::hash should be specialised
 */
template<optifol::StorableType Type>
struct std::hash<Type> // NOLINT(*-dcl58-cpp): Specialising std::hash not result in UB
{
    /**
     * @brief Compute the hash of the given storable object
     * @param storable_object The storable object to hash
     * @return The hash of the storable object
     * @todo The ID alone is rubbish hash. Can we combine it with the creation date perhaps?
     */
    std::size_t operator()(const Type& storable_object) const noexcept
    {
        return static_cast<const optifol::IStorageObject&>(storable_object).get_controller_id();
    }
};

namespace optifol
{

/**
 * @class StorageHashFunctor
 * @brief The transparent hashing functor, templated to a storable type, capable of hashing the type or a transparently
 *  provided numerical ID
 * @tparam Type The concrete storable type for which a transparent-hashing functor should be templated
 */
template<StorableType Type>
struct StorageHashFunctor
{
    /**
     * @typedef hash_type
     * @brief The underlying hasher type
     */
    using hash_type = std::hash<Type>;

    /**
     * @typedef is_transparent
     * @brief Indicate that the StorageHashFunctor will participate in transparent STL hashing
     * @see C++23 JTC standard: [associative.reqmts.general](180) regarding Compare::is_transparent
     */
    using is_transparent = void;

    /**
     * @brief Compute the hash of the storable object
     * @param storable_object The storable object to hash
     * @return The hash of the storable object
     */
    std::size_t operator()(const Type& storable_object) const noexcept
    {
        return hash_type{}(storable_object);
    }

    /**
     * @brief Compute the hash of the object represented by the given ID
     * @param storable_object_id The ID of the storable object to hash
     * @return The hash of the storable object represented by the ID
     */
    std::size_t operator()(const std::size_t storable_object_id) const noexcept
    {
        return storable_object_id;
    }

    std::size_t operator()(const Glib::RefPtr<Type>& shared_storage_object) const noexcept // TODO is this noexcept?
    {
        return hash_type{}(*shared_storage_object);
    }
};

}

#endif
