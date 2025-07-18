/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Subsystem-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef SUBSYSTEM_HPP
#define SUBSYSTEM_HPP

#include <giomm/liststore.h>

#include "AnalysisGroup.hpp"
#include "Requirement.hpp"
#include "StorageObjectBase.hpp"
#include "TreeNode.hpp"

namespace optifol
{

/**
 * @class Subsystem
 * @brief The Subsystem storage object forms the second level of the Optifol object hierarchy; it belongs to a Project,
 *  and consists of many individual requirements.
 */
class Subsystem :
        public StorageObjectBase,
        public TreeNode
{
public:
    /**
     * @brief Create a new Subsystem with the given name and register in the Glib GType system
     * @param name The initial name of the Subsystem
     * @param parent The owning node: typically a Project (if root-level Subsystem) or Subsystem if a member of a nested
     *  hierarchy.
     */
    explicit Subsystem(std::string&& name, TreeNode * parent);

    /**
     * @brief Create a new Subsystem with the given name and register in the Glib GType system
     * @param name The initial name of the Subsystem
     * @param cobject The C cast-item used by Glib::Object
     * @param builder Currently unused builder parameter to provide to the Glib::Object instance
     * @param parent The owning node: typically a Project (if root-level Subsystem) or Subsystem if a member of a nested
     *  hierarchy.
     */
    Subsystem(std::string&& name, BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder,
        TreeNode * parent);

    /**
     * @brief Tests a couple of subsystem for equality
     * @param other The other subsystem
     * @return Is the current subsystem the same as the other subsystem?
     * @note This comparator determines equality by subsystem metadata.
     */
    bool operator==(const Subsystem& other) const noexcept;

    /**
     * @brief Recursively generate a human-readable path of the Subsystem hierarchy, delimited with oblique characters
     * @return The human-readable path of the current subsystem with a leading oblique
     */
    [[nodiscard]] std::string get_path() const override;

    // TODO: shouldn't be public.
    Glib::RefPtr<Gio::ListStore<Requirement>> requirements = Gio::ListStore<Requirement>::create();

    // TODO: shouldn't be public.
    Glib::RefPtr<Gio::ListStore<AnalysisGroup>> analysis_groups = Gio::ListStore<AnalysisGroup>::create();

private:
    mutable std::pair<std::size_t, std::string> fully_qualified_path_cache;
};

}

#endif
