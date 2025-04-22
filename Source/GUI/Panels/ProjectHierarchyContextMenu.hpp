/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Project Hierarchical Context Menu in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-04-23
 * @version Development
 */

#ifndef PROJECTHIERARCHYCONTEXTMENU_HPP
#define PROJECTHIERARCHYCONTEXTMENU_HPP

#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/popovermenu.h>

namespace optifol
{

/**
 * @class ProjectHierarchyContextMenu
 * @brief The ProjectHierarchyContextMenu provides functionality to the project hierarchical view by means of a
 *  multi-function context menu with a graphical pop-up located at the cursor position within the parent widget.
 * @details The following actions are currently supported for arbitrary callbacks:
 *  <ul>
 *      <li>New Project: non-optional action to create and register a new top-level project;</li>
 *      <li>New Subsystem: optional action to create and register a new nestable subsystem;</li>
 *      <li>Edit Structure: optional action to modify the contextually defined project or subsystem; and</li>
 *      <li>Delete Structure: optional action to delete the current project or subsystem and all children.</li>
 *  </ul>
 */
class ProjectHierarchyContextMenu
{
public:

    /**
     * @brief Construct a new ProjectHierarchyContextMenu on the given parent with the given callbacks
     * @param parent The parent widget on which the context menu should be registered. This widget is enriched with a
     *  gesture and action group, prefixed with the "hierarchy" keyword, to support HCI integration with the context
     *  menu.
     * @param menu_source The Gio::Menu entity, probably loaded from a UI definitions file, containing the declarative
     *  menu description.
     * @param new_project The callback to bind to the New Project action
     * @param new_subsystem The callback to bind to the New Subsystem action
     * @param edit_structure The callback to bind to the Edit Structure action
     * @param delete_structure The callback to bind to the Delete Structure action
     */
    ProjectHierarchyContextMenu(Gtk::Widget* parent, const Glib::RefPtr<Gio::Menu>& menu_source,
        sigc::slot<void()>&& new_project, sigc::slot<void()>&& new_subsystem, sigc::slot<void()>&& edit_structure,
        sigc::slot<void()>&& delete_structure);

    /**
     * @brief Enable the optional actions
     */
    void enable_optional();

    /**
     * @brief Disable the optional actions
     */
    void disable_optional();

private:
    bool enabled = false;

    Gtk::PopoverMenu menu;
    Glib::RefPtr<Gio::SimpleActionGroup> action_group = Gio::SimpleActionGroup::create();

    Glib::RefPtr<Gio::SimpleAction> new_project;
    Glib::RefPtr<Gio::SimpleAction> new_subsystem;
    Glib::RefPtr<Gio::SimpleAction> edit_structure;
    Glib::RefPtr<Gio::SimpleAction> delete_structure;
};

}

#endif
