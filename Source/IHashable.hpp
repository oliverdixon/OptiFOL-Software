/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef IHASHABLE_HPP
#define IHASHABLE_HPP

#include <cstddef>
#include <memory>

namespace optifol
{

class IHashable
{
public:
    virtual ~IHashable() = default;

    [[nodiscard]] virtual std::size_t hash() const noexcept = 0;

protected:
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

template<typename Type>
concept IsHashable = std::derived_from<Type, IHashable>;

} // namespace optifol

template<optifol::IsHashable Type>
struct std::hash<Type> // NOLINT(*-dcl58-cpp) Specialising std::hash does not result in UB.
{
    std::size_t operator()(const Type &hashable) const noexcept
    {
        return hashable.hash();
    }

    std::size_t operator()(const std::shared_ptr<Type>& shared_hashable) const noexcept
    {
        return shared_hashable->hash();
    }
};

#endif
