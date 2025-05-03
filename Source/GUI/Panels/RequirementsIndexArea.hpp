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

#include <giomm/liststore.h>
#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>
#include <gtkmm/listitem.h>
#include <gtkmm/singleselection.h>

#include "../ContextButtonCorrespondence.hpp"
#include "../../Storage/Requirement.hpp"

namespace optifol
{

/**
 * @class RequirementsIndexArea
 * @brief The manager of the UI element representing the Requirements Index, wherein the user can view and interact with
 *  a single set of Requirement objects, grouped by the Project and Subsystem.
 */
class RequirementsIndexArea :
        public sigc::trackable
{
public:
    /**
     * @brief Construct a new compartmentalised area for displaying and managing sets of subsystem requirements
     * @param builder The GTK builder attached to the main window
     */
    explicit RequirementsIndexArea(Gtk::Builder& builder);

    void select_model(const Glib::RefPtr<Gio::ListStore<Requirement>>& new_model);

    void deselect_model();

    /**
     * @brief Bind a Requirement description attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_description(const Glib::RefPtr<Gtk::ListItem> &list_item);

    /**
     * @brief Bind a Requirement statement attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_statement(const Glib::RefPtr<Gtk::ListItem> &list_item);

    /**
     * @brief Bind a Requirement priority attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_priority(const Glib::RefPtr<Gtk::ListItem> &list_item);

private:
    Glib::RefPtr<Gio::ListStore<Requirement>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;

    static const char * const area_name;

    Gtk::ColumnView * view;

    ContextButtonCorrespondence context_menu;

    void configure_new_requirement_popover(Gtk::Builder& builder) const;

    void configure_edit_requirement_popover(Gtk::Builder& builder) const;

    void configure_delete_requirement_popover(Gtk::Builder& builder) const;

    void configure_duplicate_requirement_popover(Gtk::Builder& builder) const;
};

}

#endif
