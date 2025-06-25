/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification and implementation for the HashableEqualityFunctor
 * @author Oliver Dixon
 * @date 2025-06-25
 * @version Development
 */

#ifndef HASHABLEEQUALITYFUNCTOR_HPP
#define HASHABLEEQUALITYFUNCTOR_HPP

#include "IHashable.hpp"

namespace optifol
{

/**
 * @class HashableEqualityFunctor
 * @brief Provides an equality functor for IHashable types, including transparency to instantiations detained by
 *  @ref std::unique_ptr. Useful for transparent hashing in unordered associative STL containers.
 * @tparam Type The IHashable and equality-comparable type on which equality should be tested
 */
template<typename Type>
    requires std::derived_from<Type, IHashable> && std::equality_comparable<Type>
struct HashableEqualityFunctor
{
    using is_transparent = void;

    /**
     * @brief Test the LHS and RHS value objects for equality
     * @param lhs The left-hand-side object
     * @param rhs The right-hand-side object
     * @return Do the LHS and RHS compare to be equal?
     */
    bool operator()(const Type &lhs, const Type &rhs) const noexcept
    {
        return lhs == rhs;
    }

    /**
     * @brief Test the LHS pointer and RHS value objects for equality
     * @param lhs The left-hand-side object, detained in a @ref std::unique_ptr
     * @param rhs The right-hand-side object
     * @return Do the LHS and RHS represent objects that compare to be equal?
     */
    bool operator()(const std::unique_ptr<Type> &lhs, const Type &rhs) const noexcept
    {
        return hetro_compare(lhs, rhs);
    }

    /**
     * @brief Test the LHS value and RHS pointer objects for equality
     * @param lhs The left-hand-side object
     * @param rhs The right-hand-side object, detained in a @ref std::unique_ptr
     * @return Do the LHS and RHS represent objects that compare to be equal?
     */
    bool operator()(const Type &lhs, const std::unique_ptr<Type> &rhs) const noexcept
    {
        return hetro_compare(rhs, lhs);
    }

    /**
     * @brief Test the LHS pointer and RHS pointer objects for equality
     * @param lhs The left-hand-side object, detained in a @ref std::unique_ptr
     * @param rhs The right-hand-side object, detained in a @ref std::unique_ptr
     * @return Do the LHS and RHS represent objects that compare to be equal?
     */
    bool operator()(const std::unique_ptr<Type> &lhs, const std::unique_ptr<Type> &rhs) const noexcept
    {
        if (lhs == rhs)
            // Same pointer or both nullptr
            return true;

        if (!lhs || !rhs)
            // One nullptr and one populated
            return false;

        return *lhs == *rhs;
    }

private:
    /**
     * @brief Compare heterogeneous instantiations of the Type for equality, over pointer and value boundaries
     * @param pointer The pointer instantiation
     * @param value The value instantiation
     * @return Do the two instantiations refer to hash-equal objects?
     */
    static bool hetro_compare(const std::unique_ptr<Type> &pointer, const Type &value) noexcept
    {
        if (pointer == nullptr)
            return false;

        return *pointer == value;
    }
};

} // namespace optifol

#endif // HASHABLEEQUALITYFUNCTOR_HPP
