/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the base class of a Storable Object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef STORAGEOBJECTBASE_HPP
#define STORAGEOBJECTBASE_HPP

#include <chrono>
#include <glibmm/object.h>
#include <glibmm/property.h>
#include <gtkmm/builder.h>

#include "../IHashable.hpp"

namespace optifol
{

/**
 * @class StorageObjectBase
 * @brief Defines the base class for a Glib-registered storable object in the Optifol storage hierarchy
 */
class StorageObjectBase :
        public Glib::Object,
        public IHashable
{
public:
    /**
     * @typedef TimeT
     * @brief The internal C++ type used to manipulate times on the system clock
     */
    using TimeT = std::chrono::system_clock::time_point;

    /**
     * @brief Get a read-write proxy for the 'name' property
     * @return The read-write 'name' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_name();

    /**
     * @brief Get a read-write proxy for the 'creation time' property
     * @return The read-write 'creation time' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<TimeT> property_creation_time();

    /**
     * @brief Get a read-write proxy for the 'last-modified time' property
     * @return The read-write 'last-modified time' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<TimeT> property_modified_time();

    /**
     * @brief Get a read-only proxy for the 'name' property
     * @return The read-only 'name' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_name() const;

    /**
     * @brief Get a read-only proxy for the 'creation time' property
     * @return The read-only 'creation time' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<TimeT> property_creation_time() const;

    /**
     * @brief Get a read-only proxy for the 'last-modified time' property
     * @return The read-only 'last-modified time' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<TimeT> property_modified_time() const;

    [[nodiscard]] std::size_t hash() const noexcept override;

protected:
    /**
     * @brief Derive from the common storage base with the universal of GType-registered properties
     */
    explicit StorageObjectBase();

    /**
     * @brief Derive from the common storage base with the universal of GType-registered properties
     * @param cobject The C cast-item used by Glib::Object
     * @param builder Currently unused builder parameter to provide to the Glib::Object instance
     */
    StorageObjectBase(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

private:
    Glib::Property<Glib::ustring> name;

    Glib::Property<TimeT> creation_time;

    Glib::Property<TimeT> modified_time;
};


// Current Clang 18 bug reports Doxygen violations for uses of @tparam on templated concepts.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
/**
 * @concept StorableType
 * @brief Represents a type that is declared to be a permanently storable object in the Optifol type system
 * @tparam Type The implementing type of the storable object
 */
template<typename Type>
concept StorableType = std::derived_from<Type, StorageObjectBase>;
#pragma clang diagnostic pop

}

#endif
