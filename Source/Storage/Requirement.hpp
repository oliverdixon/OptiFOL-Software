/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the requirement-level storage object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#ifndef REQUIREMENT_HPP
#define REQUIREMENT_HPP

#include "StorageObjectBase.hpp"

namespace optifol
{

/**
 * @class Requirement
 * @brief The Requirement storage object is the atomic unit of measure in Optifol. It belongs to a single Subsystem.
 */
class Requirement :
        public StorageObjectBase
{
public:
    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system
     * @param name The initial name of the Requirement
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     */
    explicit Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority);

    /**
     * @brief Create a new Requirement with the given name and register in the Glib GType system
     * @param name The initial name of the Subsystem
     * @param statement The initial FOL statement of the Requirement
     * @param description The initial long-form description of the Requirement
     * @param priority The initial priority of the Requirement
     * @param cobject The C cast-item used by Glib::Object
     * @param builder Currently unused builder parameter to provide to the Glib::Object instance
     */
    Requirement(std::string&& name, std::string&& statement, std::string&& description, guint priority,
        BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

    /**
     * @brief Get a read-write proxy for the 'statement' property
     * @return The read-write 'statement' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_statement();

    /**
     * @brief Get a read-write proxy for the 'description' property
     * @return The read-write 'description' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_description();

    /**
     * @brief Get a read-write proxy for the 'priority' property
     * @return The read-write 'priority' proxy
     */
    [[nodiscard]] Glib::PropertyProxy<guint> property_priority();

    /**
     * @brief Get a read-only proxy for the 'statement' property
     * @return The read-only 'statement' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_statement() const;

    /**
     * @brief Get a read-only proxy for the 'description' property
     * @return The read-only 'description' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_description() const;

    /**
     * @brief Get a read-only proxy for the 'priority' property
     * @return The read-only 'priority' proxy
     */
    [[nodiscard]] Glib::PropertyProxy_ReadOnly<guint> property_priority() const;

private:
    Glib::Property<Glib::ustring> statement;

    Glib::Property<Glib::ustring> description;

    Glib::Property<guint> priority;
};

}

#endif
