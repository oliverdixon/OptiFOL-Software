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

#include "../UserTesting/TestGroup.hpp"
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
class Subsystem : public StorageObjectBase,
                  public TreeNode,
                  public RequirementGroupBase
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

    /**
     * @brief Build a new Requirement, using the forwarded arguments, with the Subsystem SymbolRepository instance. The
     *  built Requirement is immediately appended to the model.
     * @tparam CtorArgs The types of Requirement constructor arguments to forward, as a parameter pack.
     * @param args The head argument values to forward to the Requirement constructor.
     */
    template<typename... CtorArgs>
    void build_requirement(CtorArgs &&...args)
    {
        insert_requirement(Glib::make_refptr_for_instance(
            new Requirement(std::forward<CtorArgs>(args)..., symbol_repository)));
    }

    void record_slated_requirement(Glib::RefPtr<Requirement> slated_requirement, guint old_index) override;

    /**
     * @brief Duplicate the given Requirement and append to the index.
     * @param requirement The Requirement to duplicate.
     */
    void duplicate_requirement(const Requirement &requirement);

    Glib::RefPtr<Gio::ListStore<AnalysisGroup>> get_analysis_groups() const noexcept;

    Glib::RefPtr<Gio::ListStore<TestGroup>> get_test_groups() const noexcept;

private:
    /**
     * @brief Configure the Subsystem to a known initial state, including the configuration of signal handlers for
     *  changing internal list models, and construction of default test and analysis groups.
     * @param name The initial name of the Subsystem
     */
    void setup_groups(const Glib::ustring &name);

    /**
     * @brief Handle insertions and/or deletions in the flat Requirement index model
     * @param initial_index The index at which insertions/deleted started
     * @param removed_count The number of Requirement objects removed from the list
     * @param added_count The number of Requirement objects added to the list
     */
    void handle_requirement_change(guint initial_index, guint removed_count, guint added_count);

    Glib::RefPtr<Gio::ListStore<AnalysisGroup>> analysis_groups = Gio::ListStore<AnalysisGroup>::create();
    Glib::RefPtr<Gio::ListStore<TestGroup>> test_groups = Gio::ListStore<TestGroup>::create();

    std::unordered_map<guint, Glib::RefPtr<Requirement>> deleted_requirements;

    // TODO: future work - should this be in Analysis group for different group-wise FOL interpretations?
    SymbolRepository symbol_repository;

    mutable std::pair<std::size_t, std::string> fully_qualified_path_cache;
};

} // namespace optifol

#endif
