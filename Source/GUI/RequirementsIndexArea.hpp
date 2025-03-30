/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Main Window's Requirements Index UI area
 * @author Oliver Dixon
 * @date 2025-03-29
 * @version Development
 */

#ifndef REQUIREMENTSINDEXAREA_HPP
#define REQUIREMENTSINDEXAREA_HPP

#include <gtkmm.h>

#include "GTKHelpers.hpp"
#include "../Storage/PGRequirementModel.hpp"

namespace optifol
{

class RequirementsIndexArea :
        public sigc::trackable
{
public:
    RequirementsIndexArea(Gtk::ColumnView * view, const Glib::RefPtr<PGRequirementModel> &initial_model);

    void set_model(const Glib::RefPtr<PGRequirementModel>& new_model) const;

private:
    Glib::RefPtr<Gtk::SingleSelection> selection_model;

    template<typename SetterFunc>
    void on_setup_label(const Glib::RefPtr<Gtk::ListItem> &list_item, SetterFunc&& setter_function,
        bool mono_styling = false);

    std::pair<Glib::RefPtr<Requirement>, Gtk::EditableLabel*> on_bind_setup(
        const Glib::RefPtr<Gtk::ListItem>& list_item) const;

    template<typename GetterFunc>
    void on_bind_label(const Glib::RefPtr<Gtk::ListItem>& list_item, GetterFunc&& getter_function) const;

    template<mp_helpers::OptionalReturner GetterFunc>
    void on_bind_label(const Glib::RefPtr<Gtk::ListItem>& list_item, GetterFunc&& getter_function) const;

    template<typename SetterFunc>
    void on_edit_label(const Glib::RefPtr<Gtk::ListItem>& list_item, SetterFunc&& setter_function);
};

}

#endif
