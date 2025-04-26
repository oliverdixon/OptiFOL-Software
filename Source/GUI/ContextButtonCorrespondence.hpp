/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef CONTEXTBUTTONCORRESPONDENCE_HPP
#define CONTEXTBUTTONCORRESPONDENCE_HPP

#include <optional>
#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/popovermenu.h>

namespace optifol
{

class ContextButtonCorrespondence
{
public:
    explicit ContextButtonCorrespondence(Gtk::Widget * parent, const Glib::RefPtr<Gio::Menu>& menu_source,
        std::initializer_list<std::tuple<
            std::string,
            Gtk::MenuButton *,
            Gtk::Popover *,
            bool>
        >&& map);

    void enable_action(const Glib::ustring& action_name) const;

    void disable_action(const Glib::ustring& action_name) const;

private:
    std::unordered_map<std::string, Gtk::MenuButton *> map;

    Glib::RefPtr<Gio::SimpleActionGroup> action_group = Gio::SimpleActionGroup::create();

    Gtk::PopoverMenu menu;
};

}

#endif
