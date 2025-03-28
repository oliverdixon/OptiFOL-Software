/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef REQUIREMENTSINDEXAREA_HPP
#define REQUIREMENTSINDEXAREA_HPP

#include <gtkmm.h>

#include "../Storage/PGRequirementModel.hpp"

namespace optifol
{

class RequirementsIndexArea
{
public:
    RequirementsIndexArea(Gtk::ColumnView * view, const Glib::RefPtr<PGRequirementModel> &initial_model);

private:
    Glib::RefPtr<PGRequirementModel> requirements_model;

    static void on_setup_static(const Glib::RefPtr<Gtk::ListItem>& list_item);

    static void on_setup_statement(const Glib::RefPtr<Gtk::ListItem>& list_item);

    void on_setup_edit(const Glib::RefPtr<Gtk::ListItem>& list_item, Glib::SignalProxyProperty::SlotType&& edit_callback);

    template<typename F>
    void on_bind_name(const Glib::RefPtr<Gtk::ListItem>& list_item, F&& information_function) const;
};

}

#endif
