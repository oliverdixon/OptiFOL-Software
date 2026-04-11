/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Context Menu-Menu Button correspondence helper
 * @author Oliver Dixon
 * @date 2026-04-11
 * @version Development
 */

#ifndef CONTEXTBUTTONCORRESPONDENCE_HPP
#define CONTEXTBUTTONCORRESPONDENCE_HPP

#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/popovermenu.h>

namespace optifol
{

/**
 * @class ContextButtonCorrespondence
 * @brief Manage common popover-based actions linked by both a context menu item and menu button.
 */
class ContextButtonCorrespondence
{
public:
    /**
     * @brief Create a new correspondence
     * @param parent The parent widget of the PopoverMenu containing the menu option
     * @param menu_source The GTK list model populating the PopoverMenu
     * @param map A tuple containing the action name, the menu button to which it is linked, the popover linked to the
     *  action, and whether it should be enabled by default.
     */
    explicit ContextButtonCorrespondence(Gtk::Widget *parent, const Glib::RefPtr<Gio::Menu> &menu_source,
            std::initializer_list<std::tuple<std::string, Gtk::MenuButton *, Gtk::Popover *, bool>> &&map);

    /**
     * @brief Enable the named action via its menu button and context menu entry.
     * @param action_name The name of the action to enable
     */
    void enable_action(const Glib::ustring &action_name) const;

    /**
     * @brief Disable the named action via its menu button and context menu entry.
     * @param action_name The name of the action to disable
     */
    void disable_action(const Glib::ustring &action_name) const;

private:
    std::unordered_map<std::string, Gtk::MenuButton *> map;

    Glib::RefPtr<Gio::SimpleActionGroup> action_group = Gio::SimpleActionGroup::create();

    Gtk::PopoverMenu menu;
};

} // namespace optifol

#endif
