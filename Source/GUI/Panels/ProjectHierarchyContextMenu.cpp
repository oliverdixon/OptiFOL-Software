/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Project Hierarchical Context Menu in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-04-23
 * @version Development
 */

#include "ProjectHierarchyContextMenu.hpp"

#include "ProjectHierarchyPane.hpp"

namespace optifol
{

ProjectHierarchyContextMenu::ProjectHierarchyContextMenu(Gtk::Widget *parent,
        const Glib::RefPtr<Gio::Menu>& menu_source, sigc::slot<void()>&& new_project,
        sigc::slot<void()>&& new_subsystem, sigc::slot<void()>&& edit_structure, sigc::slot<void()>&& delete_structure)
{
    menu.set_parent(*parent);
    menu.set_menu_model(menu_source);

    const auto gesture = Gtk::GestureClick::create();
    gesture->set_button(GDK_BUTTON_SECONDARY);
    gesture->signal_released().connect([this](int, const double x, const double y)
    {
        const Gdk::Rectangle rect(static_cast<int>(x), static_cast<int>(y), 1, 1);
        menu.set_pointing_to(rect);
        menu.popup();
    });

    this->new_project = action_group->add_action("new_project", new_project);
    this->new_subsystem = action_group->add_action("new_subsystem", new_subsystem);
    this->edit_structure = action_group->add_action("edit_structure", edit_structure);
    this->delete_structure = action_group->add_action("delete_structure", delete_structure);

    this->new_subsystem->set_enabled(false);
    this->edit_structure->set_enabled(false);
    this->delete_structure->set_enabled(false);

    parent->insert_action_group("hierarchy", action_group);
    parent->add_controller(gesture);
}

void ProjectHierarchyContextMenu::enable_optional()
{
    if (!enabled) {
        new_subsystem->set_enabled(true);
        edit_structure->set_enabled(true);
        delete_structure->set_enabled(true);

        enabled = true;
    }
}

void ProjectHierarchyContextMenu::disable_optional()
{
    if (enabled) {
        new_subsystem->set_enabled(false);
        edit_structure->set_enabled(false);
        delete_structure->set_enabled(false);

        enabled = false;
    }
}

}
