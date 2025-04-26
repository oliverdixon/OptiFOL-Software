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

#include "../ContextButtonCorrespondence.hpp"
#include "../../Storage/Project.hpp"
#include "../../Storage/GObjects/GRequirement.hpp"

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
     * @typedef SelectedCallbackSignature
     * @brief The function signature of the callback to handle updates in subsystem selection
     */
    using SelectedCallbackSignature = void(const Glib::RefPtr<Gio::ListStore<GRequirement>>&);

    /**
     * @typedef DeselectedCallbackSignature
     * @brief The function signature of the callback to handle a subsystem being deselected
     */
    using DeselectedCallbackSignature = void();

    /**
     * @typedef RefreshStorageCallbackSignature
     * @brief The function signature of the callback to handle a request to read any changes reported by the storage
     *  backend
     */
    using RefreshStorageCallbackSignature = void();

    /**
     * @brief Construct a project pane manager given a managed Gtk::ListView resource and backend model
     * @param builder The GTK builder attached to the main window
     * @param initial_model The backend storage model used to populate the model and stream data updates
     * @param selected_subsystem_callback The callback to execute when the subsystem selection changes
     * @param deselected_subsystem_callback The callback to execute when the subsystem is deselected
     */
    ProjectHierarchyPane(Gtk::Builder& builder, const Glib::RefPtr<Gio::ListStore<Project>> &initial_model,
        sigc::slot<SelectedCallbackSignature>&& selected_subsystem_callback,
        sigc::slot<DeselectedCallbackSignature>&& deselected_subsystem_callback);

private:
    enum class ProjectStackSwitcherIdx
    {
        Explorer = 0,
        Metadata = 1
    };

    void configure_new_project_popup(Gtk::Builder &builder) const;

    void configure_new_subsystem_popup(Gtk::Builder &builder) const;

    void configure_edit_structure_popup(Gtk::Builder &builder) const;

    void configure_delete_structure_popup(Gtk::Builder &builder) const;

    /**
     * @brief GTK callback for a new Gtk::ListItem. This member function handles the configuration of a new entry in the
     *  Project Hierarchy view as an editable label.
     * @param item The new list item to configure for placement within the tree view
     */
    static void tree_node_setup(const Glib::RefPtr<Gtk::ListItem> &item);

    /**
     * @brief GTK callback for a Gtk::ListItem being bound or re-bound to the view; typically handles data content
     *  updates.
     * @param item The list item to reconfigure following a bind
     */
    void tree_node_bind(const Glib::RefPtr<Gtk::ListItem>& item) const;

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

    void switch_subsystem(guint) const;

    static const char * const area_name;
    static std::shared_ptr<log4cxx::Logger> logger;

    sigc::signal<SelectedCallbackSignature> signal_select_subsystem;
    sigc::signal<DeselectedCallbackSignature> signal_deselect_subsystem;

    Glib::RefPtr<Gtk::TreeListModel> tree_model;
    Gtk::DropDown * const stack_switcher;
    Gtk::Stack * const stack;
    Gtk::ListView * const view;

    Glib::RefPtr<Gio::ListStore<Project>> root_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model;

    ContextButtonCorrespondence context_menu;
};

}

#endif
