/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic IR Sentence interface
 * @author Oliver Dixon
 * @date 2025-06-14
 * @version Development
 */

#ifndef ISENTENCE_HPP
#define ISENTENCE_HPP

#include "../../IHashable.hpp"
#include "../../ISerialisable.hpp"
#include "../../Optifol.hpp"

namespace optifol
{

/**
 * @class ISentence
 * @brief A sentence represents a first-order logic sentence node within an IR tree. In most instantiations, it may
 *  consist of a deeply nested structure and always supports a polarity, i.e. positive or negative.
 */
class ISentence : public IHashable, public ISerialisable
{
public:
    [[nodiscard]] virtual bool is_negative_polarity() const noexcept = 0;

protected:
    /**
     * @brief Commutatively compare the LHS and RHS nodes of two objects for equality.
     * @tparam LHSType The type of the LHS nodes.
     * @tparam RHSType The type of the RHS nodes.
     * @param my_lhs The LHS of the first node.
     * @param my_rhs The RHS of the first node.
     * @param their_lhs The LHS of the second node.
     * @param their_rhs The RHS of the second node.
     * @return Does the first LHS equal the LHS or RHS of the second node, and does the first RHS equal the LHS or RHS
     *  of the second node?
     */
    template<class LHSType, class RHSType> requires WeaklyEqualityComparableWith<LHSType, RHSType>
    [[nodiscard]] static bool commutative_ptr_compare(const LHSType *const my_lhs, const RHSType *const my_rhs,
            const LHSType *const their_lhs, const RHSType *const their_rhs) noexcept
    {
        // If the first equality check fails, try flipping the "us" arguments to match the order of "their" arguments.
        return noncommutative_ptr_compare(my_lhs, my_rhs, their_lhs, their_rhs) ||
                noncommutative_ptr_compare(my_rhs, my_lhs, their_lhs, their_rhs);
    }

    /**
     * @brief Non-commutatively compare the LHS and RHS nodes of two objects for equality.
     * @tparam LHSType The type of the LHS nodes.
     * @tparam RHSType The type of the RHS nodes.
     * @param my_lhs The LHS of the first node.
     * @param my_rhs The RHS of the first node.
     * @param their_lhs The LHS of the second node.
     * @param their_rhs The RHS of the second node.
     * @return Does the first LHS equal the LHS of the second node, and does the first RHS equal the RHS of the second
     *  node?
     */
    template<class LHSType, class RHSType> requires WeaklyEqualityComparableWith<LHSType, RHSType>
    [[nodiscard]] static bool noncommutative_ptr_compare(const LHSType *const my_lhs, const RHSType *const my_rhs,
            const LHSType *const their_lhs, const RHSType *const their_rhs) noexcept
    {
        bool lhs_matches = false;

        if (my_lhs == nullptr)
            // My LHS is NULL, so equality is achieved if and only if their LHS is also NULL.
            lhs_matches = their_lhs == nullptr;
        else if (their_lhs == nullptr)
            // Their LHS is NULL, but we know that our LHS is non-NULL.
            lhs_matches = false;
        else
            // Both LHS pointers are non-NULL.
            lhs_matches = *my_lhs == *their_lhs;

        if (!lhs_matches)
            return false;

        // LHS matches. If either of the RHS pointers are NULL, equality is achieved if and only if they're both NULL.
        if (my_rhs == nullptr || their_rhs == nullptr)
            return my_rhs == their_rhs;

        // Both RHS pointers are non-NULL.
        return *my_rhs == *their_rhs;
    }
};

} // namespace optifol

#endif
