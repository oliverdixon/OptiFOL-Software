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
#include "../../Storage/Requirement.hpp"

namespace optifol
{
class Subsystem;

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
    using SelectedCallbackSignature = void(const Glib::RefPtr<Gio::ListStore<Requirement>>&);

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

    /**
     * @brief GTK callback for a new Gtk::ListItem
     * @param item The new list item to configure for placement within the tree view
     */
    static void on_setup(const Glib::RefPtr<Gtk::ListItem> &item);

    /**
     * @brief GTK callback for a Gtk::ListItem being bound or re-bound to the view; typically handles data content
     *  updates
     * @param item The list item to reconfigure following a bind
     */
    void on_bind(const Glib::RefPtr<Gtk::ListItem>& item) const;

    void on_dropdown_changed() const;

    /**
     * @brief Retrieves sub-models for tree items being expanded
     * @param item The item being expanded
     * @return The model representing the children of the expanded object
     */
    Glib::RefPtr<Gio::ListModel> on_expand(const Glib::RefPtr<Glib::ObjectBase> &item) const;

    static const char * const area_name;

    sigc::signal<SelectedCallbackSignature> signal_select_subsystem;
    sigc::signal<DeselectedCallbackSignature> signal_deselect_subsystem;

    static std::shared_ptr<log4cxx::Logger> logger;
    Glib::RefPtr<Gtk::TreeListModel> tree_model;
    Gtk::DropDown * const stack_switcher;
    Gtk::Stack * const stack;

    Glib::RefPtr<Gio::ListStore<Project>> model;
};

}

#endif
