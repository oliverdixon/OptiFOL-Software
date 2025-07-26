/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Project Hierarchical view in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#ifndef PROJECTHIERARCHYPANE_HPP
#define PROJECTHIERARCHYPANE_HPP

#include <gtkmm.h>
#include <log4cxx/logger.h>

#include "../../Storage/Project.hpp"
#include "../AnalysisArea/AnalysisArea.hpp"
#include "../ContextButtonCorrespondence.hpp"

namespace optifol
{

/**
 * @class ProjectHierarchyPane
 * @brief The manager of the UI view element representing an interactable tree of all projects and subsystems, typically
 *  derived from a backend storage controller.
 */
class ProjectHierarchyPane
{
public:
    /**
     * @brief Construct a project pane manager given a managed Gtk::ListView resource and backend model
     * @param builder The GTK builder attached to the main window
     * @param initial_model The backend storage model used to populate the model and stream data updates
     */
    ProjectHierarchyPane(Gtk::Builder &builder, const Glib::RefPtr<Gio::ListStore<Project>> &initial_model);

    /**
     * @brief Add a new listener whose callback routine should be called when a new Subsystem is selected, or the
     *  previously selected one is deselected.
     * @param selected The callback to handle the changing of the active selection to a new Subsystem
     * @param deselected The callback to handle the deselection of the currently selected Subsystem
     * @param onboard Should the newly added callback be invoked immediately to "onboard" the listener as to the current
     *  state of the selection? The deselection callback is called during onboarding if and only if the present
     *  selection is invalid or not a Subsystem.
     */
    void add_subsystem_change_callback(sigc::slot<void(const Glib::RefPtr<Subsystem> &)> &&selected,
            sigc::slot<void()> &&deselected, bool onboard = true);

private:
    enum class ProjectStackSwitcherIdx
    {
        Explorer = 0,
        Metadata = 1
    };

    /**
     * @brief Configure the sub-widgets of the 'New Project' popover
     * @param builder The builder associated with the popover
     */
    void configure_new_project_popover(Gtk::Builder &builder) const;

    /**
     * @brief Configure the sub-widgets of the 'New Subsystem' popover
     * @param builder The builder associated with the popover
     */
    void configure_new_subsystem_popover(Gtk::Builder &builder) const;

    /**
     * @brief Configure the sub-widgets of the 'Edit Structure' popover
     * @param builder The builder associated with the popover
     */
    void configure_edit_structure_popover(Gtk::Builder &builder) const;

    /**
     * @brief Configure the sub-widgets of the 'Delete Structure' popover
     * @param builder The builder associated with the popover
     */
    void configure_delete_structure_popover(Gtk::Builder &builder) const;

    /**
     * @brief GTK callback for a new Gtk::ListItem. This member function handles the configuration of a new entry in the
     *  Project Hierarchy view as an editable label.
     * @param item The new list item to configure for placement within the tree view
     */
    static void tree_node_setup(const Glib::RefPtr<Gtk::ListItem> &item);

    /**
     * @brief GTK callback for a change of selection on the stack-switcher dropdown. The current state is checked, and
     *  the corresponding stack page is made visible.
     */
    void switch_visible_stack() const;

    /**
     * @brief Retrieves sub-models for tree items being expanded
     * @param item The item being expanded
     * @return The model representing the children of the expanded object
     */
    static Glib::RefPtr<Gio::ListModel> tree_node_expand(const Glib::RefPtr<Glib::ObjectBase> &item);

    /**
     * @brief Emits a notification to all listeners that a new subsystem has been selected
     */
    void emit_selected(const Glib::RefPtr<Subsystem> &new_subsystem) const;

    /**
     * @brief Emits a notification to all listeners that the previously selected subsystem has been deselected.
     */
    void emit_deselected() const;

    /**
     * @brief Handle a selection change in the Project Hierarchy Pane by updating any internal state and informing
     *  listeners
     */
    void switch_selection(guint) const;

    static const char *const area_name;
    static std::shared_ptr<log4cxx::Logger> logger;

    std::vector<std::pair<sigc::signal<void(const Glib::RefPtr<Subsystem> &)>, sigc::signal<void()>>>
            subsystem_change_callbacks;

    Gtk::DropDown *const stack_switcher;
    Gtk::Stack *const stack;
    Gtk::ListView *const view;

    Glib::RefPtr<Gio::ListStore<Project>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model;
    const Glib::RefPtr<Gtk::TreeListModel> tree_model;

    ContextButtonCorrespondence context_menu;
};

} // namespace optifol

#endif
