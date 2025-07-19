/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/19/25.
//

#ifndef DEREFERENCINGEQUALITYFUNCTOR_HPP
#define DEREFERENCINGEQUALITYFUNCTOR_HPP

#include <utility>

namespace optifol
{

template<typename CandidateType, typename BaseType>
concept Dereferencable =
    requires(CandidateType& candidate)
{
    { *candidate } -> std::convertible_to<BaseType&>;
};

template<typename PointerType, typename BaseType> requires Dereferencable<PointerType, BaseType>
struct DereferencingEqualityFunctor
{
    using is_transparent = void;

    template<class ObjectType>
    bool operator()(PointerType lhs, ObjectType &&rhs) const
    {
        return std::forward<BaseType>(*lhs) == std::forward<ObjectType>(rhs);
    }

    bool operator()(PointerType lhs, PointerType rhs) const
    {
        return std::forward<BaseType>(*lhs) == std::forward<BaseType>(*rhs);
    }

    template<class ObjectType>
    bool operator()(ObjectType &&lhs, PointerType rhs) const
    {
        return std::forward<ObjectType>(lhs) == std::forward<BaseType>(*rhs);
    }

    template<class ObjectType>
    bool operator()(ObjectType &&lhs, ObjectType &&rhs) const
    {
        return std::forward<ObjectType>(lhs) == std::forward<ObjectType>(rhs);
    }
};

} // namespace optifol

#endif // DEREFERENCINGEQUALITYFUNCTOR_HPP
