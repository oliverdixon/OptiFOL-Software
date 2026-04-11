/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Context Menu-Menu Button correspondence helper
 * @author Oliver Dixon
 * @date 2026-04-11
 * @version Development
 */

#include <gtkmm/gestureclick.h>

#include "ContextButtonCorrespondence.hpp"

namespace optifol
{

ContextButtonCorrespondence::ContextButtonCorrespondence(Gtk::Widget *parent,
        const Glib::RefPtr<Gio::Menu> &menu_source,
        std::initializer_list<std::tuple<std::string, Gtk::MenuButton *, Gtk::Popover *, bool>> &&map)
{
    /*
     * Establish the mapping between popover-raising actions and corresponding MenuButtons. If a popover
     * constructor has been defined, call that immediately prior to the popup call.
     */
    for (const auto &[name, menu_button, popover, enabled]: map) {
        const auto action_source = action_group->add_action(name, [popover] { popover->popup(); });

        this->map.emplace(name, menu_button);
        menu_button->set_popover(*popover);
        menu_button->set_sensitive(enabled);
        action_source->set_enabled(enabled);
    }

    /*
     * Set up a callback for the changing state of actions. When the state of an underlying Gio::SimpleAction
     * is modified, this callback is triggered to locate the corresponding Gtk::MenuButton and toggle its
     * sensitivity accordingly. Therefore, the graphical button state always matches the availability of the
     * action.
     */
    action_group->signal_action_enabled_changed().connect(
            [this](const Glib::ustring &action_name, const bool enabled)
            {
                const auto map_it = this->map.find(action_name);
                if (map_it != this->map.cend())
                    map_it->second->set_sensitive(enabled);
            });

    // Set up the PopoverMenu context menu, including registration with the parent Widget.
    menu.set_parent(*parent);
    menu.set_menu_model(menu_source);

    const auto gesture = Gtk::GestureClick::create();
    gesture->set_button(GDK_BUTTON_SECONDARY);
    gesture->signal_released().connect(
            [this](int, const double x, const double y)
            {
                const Gdk::Rectangle rect(static_cast<int>(x), static_cast<int>(y), 1, 1);
                menu.set_pointing_to(rect);
                menu.popup();
            });

    parent->insert_action_group("win", action_group);
    parent->add_controller(gesture);
}

void ContextButtonCorrespondence::enable_action(const Glib::ustring &action_name) const
{
    const auto action =
            std::dynamic_pointer_cast<Gio::SimpleAction>(action_group->lookup_action(action_name));
    if (action != nullptr)
        action->set_enabled(true);
}

void ContextButtonCorrespondence::disable_action(const Glib::ustring &action_name) const
{
    const auto action =
            std::dynamic_pointer_cast<Gio::SimpleAction>(action_group->lookup_action(action_name));
    if (action != nullptr)
        action->set_enabled(false);
}

} // namespace optifol
