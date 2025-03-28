/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Project Hierarchical view in the OptiFOL Main Window
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#ifndef PROJECTHIERARCHYPANE_HPP
#define PROJECTHIERARCHYPANE_HPP

#include <gtkmm.h>
#include <log4cxx/logger.h>

#include "../Storage/PGProjectModel.hpp"

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
     * @param view The list view UI element into which the entries are rendered
     * @param initial_model The backend storage model used to populate the model and stream data updates
     */
    ProjectHierarchyPane(Gtk::ListView * view, const Glib::RefPtr<PGProjectModel> &initial_model);

private:
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

    /**
     * @brief GTK callback for expanding a node and producing the child model
     * @param item The item representing the parent of the desired child model
     * @return The child model of the given item, or nullptr if the given item is a leaf node
     */
    Glib::RefPtr<Gio::ListModel> on_expand(const Glib::RefPtr<Glib::ObjectBase>& item) const;

    static std::shared_ptr<log4cxx::Logger> logger;
    Glib::RefPtr<Gtk::TreeListModel> tree_model;
    Glib::RefPtr<PGProjectModel> project_model;
};

}

#endif
