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
#include <gtkmm/singleselection.h>

#include "AnalysisGroup.hpp"
#include "Requirement.hpp"
#include "StorageObjectBase.hpp"
#include "TestGroup.hpp"
#include "TreeNode.hpp"

namespace optifol
{

/**
 * @class Subsystem
 * @brief The Subsystem storage object forms the second level of the Optifol object hierarchy; it belongs to a Project,
 *  and consists of many individual requirements.
 */
class Subsystem : public StorageObjectBase,
                  public TreeNode
{
public:
    /**
     * @brief Create a new Subsystem with the given name and register in the Glib GType system
     * @param name The initial name of the Subsystem
     * @param parent The owning node: typically a Project (if root-level Subsystem) or Subsystem if a member of a nested
     *  hierarchy.
     */
    explicit Subsystem(const Glib::ustring &name, TreeNode *parent);

    /**
     * @brief Create a new Subsystem with the given name and register in the Glib GType system
     * @param name The initial name of the Subsystem
     * @param cobject The C cast-item used by Glib::Object
     * @param builder Currently unused builder parameter to provide to the Glib::Object instance
     * @param parent The owning node: typically a Project (if root-level Subsystem) or Subsystem if a member of a nested
     *  hierarchy.
     */
    Subsystem(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder,
            TreeNode *parent);

    /**
     * @brief Tests a couple of subsystem for equality
     * @param other The other subsystem
     * @return Is the current subsystem the same as the other subsystem?
     * @note This comparator determines equality by subsystem metadata.
     */
    bool operator==(const Subsystem &other) const noexcept;

    /**
     * @brief Recursively generate a human-readable path of the Subsystem hierarchy, delimited with oblique characters
     * @return The human-readable path of the current subsystem with a leading oblique
     */
    [[nodiscard]] std::string get_path() const override;

    // TODO: shouldn't be public.
    Glib::RefPtr<Gio::ListStore<AnalysisGroup>> analysis_groups = Gio::ListStore<AnalysisGroup>::create();

    // TODO: shouldn't be public.
    Glib::RefPtr<Gio::ListStore<TestGroup>> test_groups = Gio::ListStore<TestGroup>::create();

    void use_requirements_selection_model(Gtk::SingleSelection &target_selection_model) const;

    void for_each_requirement(const std::function<void(const Requirement &)> &function) const;

private:
    /**
     * @brief Configure the Subsystem to a known initial state, including the configuration of signal handlers for
     *  changing internal list models, and construction of default test and analysis groups.
     * @param name The initial name of the Subsystem
     */
    void setup_groups(const Glib::ustring &name);

    Glib::RefPtr<Gio::ListStore<Requirement>> requirements = Gio::ListStore<Requirement>::create();

    mutable std::pair<std::size_t, std::string> fully_qualified_path_cache;
};

} // namespace optifol

#endif
