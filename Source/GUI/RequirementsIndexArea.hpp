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

    static void on_setup_static(const Glib::RefPtr<Gtk::ListItem>& list_item);

    static void on_setup_static_mono(const Glib::RefPtr<Gtk::ListItem>& list_item);

    void on_setup_edit(const Glib::RefPtr<Gtk::ListItem>& list_item, Glib::SignalProxyProperty::SlotType&& edit_callback);

    std::pair<Glib::RefPtr<Requirement>, Gtk::Label*> on_bind_setup(const Glib::RefPtr<Gtk::ListItem>& list_item) const;

    template<typename InfoFunc>
    void on_bind_label(const Glib::RefPtr<Gtk::ListItem>& list_item, InfoFunc&& information_function) const;

    template<mp_helpers::OptionalReturner InfoFunc>
    void on_bind_label(const Glib::RefPtr<Gtk::ListItem>& list_item, InfoFunc&& information_function) const;
};

}

#endif
