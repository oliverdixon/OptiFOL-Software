/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification of the Glib storable object model base
 * @author Oliver Dixon
 * @date 2025-04-05
 * @version Development
 */

#ifndef GLIBSTORABLEOBJECTMODELBASE_HPP
#define GLIBSTORABLEOBJECTMODELBASE_HPP

#include <giomm/liststore.h>

#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class GlibStorableObjectModelBase
 * @brief The Glib-specialised implementation base for the object model.
 * @details This class specifies a uniform interface for all Glib-based object storage models and establishes an
 *  appropriate base for usage in Glib-aware containers, such as GTK viewing components. Note that due to the nature of
 *  GTK, these models are quick to iterate but slow to query; if excessive querying is required, try to use an
 *  alternative model interpretation utilising a constant-lookup-time STL set or map.
 * @tparam Type The concrete type of the objects to be stored
 */
template<StorableType Type>
class GlibStorableObjectModelBase :
        public Gio::ListStore<Type>
{
protected:
    /**
     * @brief Query the model for the implementation-defined index of a particular project in the model
     * @param object A full instantiation of the sought object
     * @return The position of the requested object, or an empty container if no suitable object exists in the model
     * @warning This function will likely take linear time in the size of the model
     */
    virtual std::optional<guint> find_object_position(const Type& object) const = 0;

    /**
     * @brief Query the model for the implementation-defined index of a particular project in the model
     * @param object_id The ID of the sought object
     * @return The position of the requested object, or an empty container if no suitable object exists in the model
     * @warning This function will likely take linear time in the size of the model
     */
    virtual std::optional<guint> find_object_position(std::size_t object_id) const = 0;
};

}

#endif
