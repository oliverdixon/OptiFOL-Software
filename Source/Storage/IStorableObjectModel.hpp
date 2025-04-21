/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition for the generic backend-independent storable object model
 * @author Oliver Dixon
 * @date 2025-04-05
 * @version Development
 */

#ifndef ISTORABLEOBJECTMODEL_HPP
#define ISTORABLEOBJECTMODEL_HPP

#include <glibmm/refptr.h>

#include "IStorageObject.hpp"

namespace optifol
{

/**
 * @class IStorableObjectModel
 * @brief Generic semantic basis for all storage models. They are moveable non-copyable complex structures.
 * @tparam Type The concrete type of the objects to be stored
 */
template<StorableType Type>
class IStorableObjectModel
{
public:
    /**
     * @brief Default constructor
     */
    IStorableObjectModel() = default;

    /**
     * @brief Default virtual destructor
     */
    virtual ~IStorableObjectModel() = default;


    /**
     * @brief Disabled copy constructor
     */
    IStorableObjectModel(const IStorableObjectModel&) = delete;

    /**
     * @brief Disabled copy-assignment operator
     * @return Syntactically: the copied model; semantically: nothing, as member function is deleted.
     */
    IStorableObjectModel& operator=(const IStorableObjectModel&) = delete;

    /**
     * @brief Model move constructor
     * @param model The model to move
     */
    IStorableObjectModel(IStorableObjectModel&& model) = default;

    /**
     * @brief Model move-assignment operator
     * @param model The model to move
     * @return The moved model
     */
    IStorableObjectModel& operator=(IStorableObjectModel&& model) = default;

    /**
     * @brief Register a new object into the model
     * @param object An r-value consumable reference to the new object
     */
    virtual void register_object(Glib::RefPtr<Type>&& object) = 0;
    
    /**
     * @brief Retrieve an existing object, identified by an instantiation of the target object, from the model
     * @param object The object to locate and return
     * @return The requested object, or an empty shared ref-counted pointer if no suitable object exists in the model
     */
    [[nodiscard]] virtual Glib::RefPtr<Type> get_object(const Type& object) = 0;
    
    /**
     * @brief Retrieve an existing object, identified by the numerical ID of the target object, from the model
     * @param object_id The numerical ID of the object to locate and return
     * @return The requested object, or an empty shared ref-counted pointer if no suitable object exists in the model
     */
    [[nodiscard]] virtual Glib::RefPtr<Type> get_object(std::size_t object_id) = 0;
    
    /**
     * @brief Remove an existing object, identified by the numerical ID of the target object, from the model
     * @param object_id The numerical ID of the object to remove
     * @return Was the entity removed?
     */
    virtual bool remove_object(std::size_t object_id) = 0;
};

}

#endif
