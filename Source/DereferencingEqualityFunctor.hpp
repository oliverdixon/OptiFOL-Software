/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification and implementation for the DereferencingEqualityFunctor
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef DEREFERENCINGEQUALITYFUNCTOR_HPP
#define DEREFERENCINGEQUALITYFUNCTOR_HPP

#include <utility>

namespace optifol
{

// Current Clang 18 bug reports Doxygen violations for uses of @tparam on templated concepts.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
/**
 * @concept Dereferencable
 * @brief Represents a type that may be dereferenced to a known base with the <code>operator*</code> member function.
 * @tparam CandidateType The outermost dereferencable candidate type.
 * @tparam BaseType The inner type resolved by <code>CandidateType::operator*()</code>.
 */
template<typename CandidateType, typename BaseType>
concept Dereferencable = requires(CandidateType& candidate)
{
    { *candidate } -> std::convertible_to<BaseType&>;
};
#pragma clang diagnostic pop

/**
 * @brief Provide a partially type-deducing functor similar to @ref std::equal_to to determine equality through
 *  dereferencable copyable type, such as a raw C-style pointer or @ref std::shared_ptr.
 *
 * @details The dereferencable and corresponding base type must be specified in template parameters, but the object type
 *  can be deduced. This functor is valid only if <code>bool ObjectType::operator==(BaseType&)</code> is defined. This
 *  functor also participates in transparent hashing for heterogeneous lookup in unordered STL containers.
 *
 * @tparam PointerType The dereferencable type, e.g. <code>const T*</code>.
 * @tparam BaseType The resolved type after dereference, e.g. <code>T</code>.
 */
template<typename PointerType, typename BaseType> requires Dereferencable<PointerType, BaseType>
struct DereferencingEqualityFunctor
{
    using is_transparent = void;

    /**
     * @brief Perform a transparent equality check for the resolved LHS and given RHS
     * @tparam ObjectType The deduced type of the RHS object
     * @param lhs A pointer referencing the LHS comparison object
     * @param rhs The RHS comparison object
     * @return Do LHS and RHS resolve to equal objects?
     */
    template<class ObjectType>
    bool operator()(PointerType lhs, ObjectType &&rhs) const
    {
        return std::forward<BaseType>(*lhs) == std::forward<ObjectType>(rhs);
    }

    /**
     * @brief Perform a transparent equality check for the resolved LHS and RHS
     * @param lhs A pointer referencing the LHS comparison object
     * @param rhs A pointer referencing the RhS comparison object
     * @return Do LHS and RHS resolve to equal objects?
     */
    bool operator()(PointerType lhs, PointerType rhs) const
    {
        return std::forward<BaseType>(*lhs) == std::forward<BaseType>(*rhs);
    }

    /**
     * @brief Perform a transparent equality check for the given LHS and resolved RHS
     * @tparam ObjectType The deduced type of the LHS object
     * @param lhs The LHS comparison object
     * @param rhs A pointer referencing the RhS comparison object
     * @return Do LHS and RHS resolve to equal objects?
     */
    template<class ObjectType>
    bool operator()(ObjectType &&lhs, PointerType rhs) const
    {
        return std::forward<ObjectType>(lhs) == std::forward<BaseType>(*rhs);
    }


    /**
     * @brief Perform a transparent equality check for the given LHS and given RHS
     * @tparam ObjectType The deduced type of the objects
     * @param lhs The LHS comparison object
     * @param rhs The RHS comparison object
     * @return Do LHS and RHS resolve to equal objects?
     */
    template<class ObjectType>
    bool operator()(ObjectType &&lhs, ObjectType &&rhs) const
    {
        return std::forward<ObjectType>(lhs) == std::forward<ObjectType>(rhs);
    }
};

} // namespace optifol

#endif // DEREFERENCINGEQUALITYFUNCTOR_HPP
