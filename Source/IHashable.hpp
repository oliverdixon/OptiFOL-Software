/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the IHashable interface
 * @author Oliver Dixon
 * @date 2025-06-09
 * @version Development
 */

#ifndef IHASHABLE_HPP
#define IHASHABLE_HPP

#include <chrono>
#include <cstddef>
#include <memory>

namespace optifol
{

class IHashable;

template<typename Candidate>
concept HashableIterator = std::bidirectional_iterator<Candidate> &&
        std::is_const_v<std::remove_reference_t<decltype(*std::declval<Candidate>())>> &&
        requires(const Candidate &candidate) {
            { (*candidate)->hash() } -> std::convertible_to<std::size_t>;
        };

/**
 * @class IHashable
 * @brief An IHashable class can be hashed, such that a (relatively) content-dependent numerical hashcode can be
 *  generated for any instance. Hashcodes are useful for implementing equality functor, but IHashable makes no
 *  guarantees on the collision properties of the generated hashcodes.
 */
class IHashable
{
public:
    /**
     * @brief Destruct an IHashable instance
     */
    virtual ~IHashable() = default;

    /**
     * @brief Produce a hash value of the IHashable object
     * @return A hash value for the object
     */
    [[nodiscard]] virtual std::size_t hash() const noexcept = 0;

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

protected:
    /**
     * @brief Commutatively combine two hashes using sensible constants, inspired by boost::hash_combine.
     * @param lhs The LHS hash value
     * @param rhs The RHS hash value
     * @return The LHS-RHS combined hash value such that LHS-RHS hash equals the equivalent RHS-LHS hash
     *  (property of commutativity).
     */
    static std::size_t hash_combine_commutative(std::size_t lhs, std::size_t rhs)
    {
        /*
         * The choice of operator> to collapse the hash operands into a commutative pair is arbitrary. We just need
         * something reliable and universally defined on std::size_t.
         */
        if (lhs > rhs)
            std::swap(lhs, rhs);

        return hash_combine(lhs, rhs);
    }

    /**
     * @brief Mutate a hash according the polarity of the node instantiation being hashed
     * @param hash The produced hash for the unsigned node (i.e., the node without a polarity)
     * @param is_negative Does the instantiation have a negative sign?
     * @return If positive, the original hash. If negative, a mutated hash to reflect the difference in polarity.
     */
    static std::size_t hash_polarity(const std::size_t hash, const bool is_negative)
    {
        // In the negative case, use MurmurHash3 as the XOR constant, and shift as usual.
        return is_negative ? hash ^ 0x85ebca6b + (hash << 6) + (hash >> 2) : hash;
    }

    /**
     * @brief Produce a hash value of a composite IR IHashable node by iterative application of
     *  @ref hash_combine(std::size_t, std::size_t).
     * @tparam Iterator @ref HashableIterator over the composed (e.g. argument) collection
     * @param symbol_name Symbol display name
     * @param composite_begin Beginning iterator of the composed collection
     * @param composite_end Ending iterator of the composed collection (one past last item)
     * @param is_negative_polarity Is the symbol instantiated with a negative polarity?
     * @return Combined hash value unique over the symbol name and all composed arguments
     */
    template<HashableIterator Iterator>
    static std::size_t composite_hash(const std::string &symbol_name, const Iterator composite_begin,
            const Iterator composite_end, const bool is_negative_polarity = false)
    {
        std::size_t hashcode = std::hash<std::string>{}(symbol_name);

        for (auto composite_it = composite_begin; composite_it != composite_end; ++composite_it)
            hashcode = hash_combine(hashcode, (*composite_it)->hash());

        return hash_polarity(hashcode, is_negative_polarity);
    }
};

} // namespace optifol

// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class std::hash<Type>
 * @brief Standard hasher implementation for Optifol's IHashable derived classes
 * @tparam Type The IHashable type to hash
 */
template<typename Type>
    requires std::derived_from<Type, optifol::IHashable>
struct std::hash<Type> // NOLINT(*-dcl58-cpp) Specialising std::hash for non-standard types does not result in UB.
{
    using is_transparent = void;

    /**
     * @brief Execute the hash functor to produce a hashcode of the object
     * @param hashable The hashable object for which a hashcode should be generated
     * @return The hashcode of the hashable object
     */
    std::size_t operator()(const Type &hashable) const noexcept
    {
        return hashable.hash();
    }

    /**
     * @brief Execute the hash functor to produce a hashcode of the object contained within the ref-counted pointer
     * @param shared_hashable The ref-counted pointer containing the hashable object for which a hashcode should be
     *  generated
     * @return The hashcode of the hashable object detained within the ref-counted pointer
     */
    std::size_t operator()(const std::shared_ptr<Type> &shared_hashable) const
    {
        return shared_hashable->hash();
    }

    /**
     * @brief Execute the hash functor to produce a hashcode of the object contained within the unique pointer
     * @param unique_hashable The unique pointer containing the hashable object for which a hashcode should be generated
     * @return The hashcode of the hashable object detained within the unique pointer
     */
    std::size_t operator()(const std::unique_ptr<Type> &unique_hashable) const
    {
        return unique_hashable->hash();
    }
};

#if __cpp_lib_chrono < 202306L

// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class std::hash<std::chrono::system_clock::time_point>
 * @brief Standard hasher specialisation for the system clock, only required prior to C++26.
 */
template<>
struct std::hash<std::chrono::system_clock::time_point>
{
    std::size_t operator()(const std::chrono::system_clock::time_point& time) const noexcept;
};

#endif

#endif
