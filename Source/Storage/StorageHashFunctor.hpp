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

#include "StorageObjectBase.hpp"

#if __cpp_lib_chrono < 202306L

// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class std::hash<std::chrono::system_clock::time_point>
 * @brief Standard hasher specialisation for the system clock, only required prior to C++26.
 */
template<>
struct std::hash<std::chrono::system_clock::time_point>
{
    std::size_t operator()(const std::chrono::system_clock::time_point& time) const noexcept
    {
        return static_cast<std::size_t>(time.time_since_epoch().count());
    }
};

#endif

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
     * @brief Compute the hash of the given storable object using the string identifier and creation time
     * @param storable_object The storable object to hash
     * @return The hash of the storable object
     */
    std::size_t operator()(const Type& storable_object) const noexcept
    {
        const auto& object = static_cast<const optifol::StorageObjectBase&>(storable_object);
        return hash_combine(std::hash<std::string>{}(object.property_name().get_value()),
            std::hash<std::chrono::system_clock::time_point>{}(object.property_creation_time().get_value()));
    }

private:
    /**
     * @brief Combine two hashes using sensible constants, inspired by boost::hash_combine.
     * @param lhs The LHS hash value
     * @param rhs The RHS hash value
     * @return The LHS-RHS combined hash value
     */
    static std::size_t hash_combine(std::size_t lhs, const std::size_t rhs)
    {
        if constexpr (sizeof(std::size_t) >= 8)
            // For 64-bit+ platforms, use the expansion of pi as the constant; it is odd at 64 bits.
            lhs ^= rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2);
        else
            // Otherwise, use the inverse of the golden ratio as a 32-bit fixed point fraction.
            // ReSharper disable once CppDFAUnreachableCode
            lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);

        return lhs;
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
     * @brief Compute the hash of the storable object
     * @param storable_object The storable object to hash
     * @return The hash of the storable object
     */
    std::size_t operator()(const Type& storable_object) const noexcept
    {
        return hash_type{}(storable_object);
    }

    std::size_t operator()(const Glib::RefPtr<Type>& shared_storage_object) const noexcept
    {
        return hash_type{}(*shared_storage_object);
    }
};

}

#endif
