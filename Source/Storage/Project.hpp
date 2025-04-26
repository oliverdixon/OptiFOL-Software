/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the project-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "StorageObjectBase.hpp"
#include "TreeNode.hpp"

namespace optifol
{
/**
 * @class Project
 * @brief The Project storage forms the top level of the Optifol object hierarchy; it contains many subsystems.
 */
class Project :
        public StorageObjectBase,
        public TreeNode
{
public:
    /**
     * @brief Create a new Project with the given name and register in the Glib GType system
     * @param name The initial name of the Project
     */
    explicit Project(std::string&& name);

    /**
     * @brief Create a new Project with the given name and register in the Glib GType system
     * @param name The initial name of the Project
     * @param cobject The C cast-item used by Glib::Object
     * @param builder Currently unused builder parameter to provide to the Glib::Object instance
     */
    Project(std::string&& name, BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

    /**
     * @brief Tests a couple of projects for equality
     * @param other The other project
     * @return Is the current project the same as the other project?
     * @note This comparator determines equality by project metadata.
     */
    bool operator==(const Project& other) const noexcept;

    /**
     * @brief Generate a path for a root-level Project, prepended with an oblique
     * @return The project path prefix
     */
    [[nodiscard]] std::string get_path() const override;
};

}

#endif
