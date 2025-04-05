/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class template for the equality functor of a storable object
 * @author Oliver Dixon
 * @date 2025-03-21
 * @version Development
 */

#ifndef STORAGEEQUALITYFUNCTOR_HPP
#define STORAGEEQUALITYFUNCTOR_HPP

#include "IStorageObject.hpp"

namespace optifol {

/**
 * @class StorageEqualityFunctor
 * @brief The transparent equality functor, templated to a storable type, capable of hashing the type, a transparently
 *  provided numerical ID, or a transparent provided Glib shared reference to the type.
 * @tparam Type The concrete storable type for which a transparent-hashing functor should be templated
 */
template<StorableType Type>
struct StorageEqualityFunctor
{

    /**
     * @typedef is_transparent
     * @brief Enable participation in STL containers utilising transparent lookup
     */
    using is_transparent = void;

    /**
     * @brief Compare concrete with concrete
     * @param lhs The concrete LHS
     * @param rhs The concrete RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const Type &lhs, const Type &rhs) const
    {
        return lhs.get_controller_id() == rhs.get_controller_id();
    }

    /**
     * @brief Compare concrete with transparent
     * @param lhs The concrete LHS
     * @param rhs The transparent RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const Type &lhs, const std::size_t rhs) const
    {
        return lhs.get_controller_id() == rhs;
    }

    /**
     * @brief Compare transparent with concrete
     * @param lhs The transparent LHS
     * @param rhs The concrete RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const std::size_t lhs, const Type &rhs) const
    {
        return lhs == rhs.get_controller_id();
    }

    /**
     * @brief Compare wrapped with concrete
     * @param lhs The wrapped LHS
     * @param rhs The concrete RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const Glib::RefPtr<Type> &lhs, const Type &rhs) const
    {
        return lhs->get_controller_id() == rhs.get_controller_id();
    }

    /**
     * @brief Compare concrete with wrapped
     * @param lhs The concrete LHS
     * @param rhs The wrapped RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const Type &lhs, const Glib::RefPtr<Type> &rhs) const
    {
        return lhs.get_controller_id() == rhs->get_controller_id();
    }

    /**
     * @brief Compare wrapped with wrapped
     * @param lhs The wrapped LHS
     * @param rhs The wrapped RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const Glib::RefPtr<Type> &lhs, const Glib::RefPtr<Type> &rhs) const
    {
        return lhs->get_controller_id() == rhs->get_controller_id();
    }

    /**
     * @brief Compare wrapped with transparent
     * @param lhs The wrapped LHS
     * @param rhs The transparent RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const Glib::RefPtr<Type> &lhs, const std::size_t rhs) const
    {
        return lhs->get_controller_id() == rhs;
    }

    /**
     * @brief Compare transparent with wrapped
     * @param lhs The transparent LHS
     * @param rhs The wrapped RHS
     * @return Is LHS semantically identical to RHS?
     */
    bool operator()(const std::size_t lhs, const Glib::RefPtr<Type> &rhs) const
    {
        return lhs == rhs->get_controller_id();
    }
};

}

#endif
