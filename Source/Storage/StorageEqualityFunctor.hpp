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
    using is_transparent = void;

    bool operator()(const Type &lhs, const Type &rhs) const
    {
        return lhs.get_controller_id() == rhs.get_controller_id();
    }

    bool operator()(const Type &lhs, const std::size_t rhs) const
    {
        return lhs.get_controller_id() == rhs;
    }

    bool operator()(const std::size_t lhs, const Type &rhs) const
    {
        return lhs == rhs.get_controller_id();
    }

    bool operator()(const Glib::RefPtr<Type> &lhs, const Type &rhs) const
    {
        return lhs->get_controller_id() == rhs.get_controller_id();
    }

    bool operator()(const Type &lhs, const Glib::RefPtr<Type> &rhs) const
    {
        return lhs.get_controller_id() == rhs->get_controller_id();
    }

    bool operator()(const Glib::RefPtr<Type> &lhs, const Glib::RefPtr<Type> &rhs) const
    {
        return lhs->get_controller_id() == rhs->get_controller_id();
    }

    bool operator()(const Glib::RefPtr<Type> &lhs, const std::size_t rhs) const
    {
        return lhs->get_controller_id() == rhs;
    }

    bool operator()(const std::size_t lhs, const Glib::RefPtr<Type> &rhs) const
    {
        return lhs == rhs->get_controller_id();
    }
};

}

#endif
