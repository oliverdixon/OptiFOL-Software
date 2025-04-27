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
#include <gtkmm/label.h>
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

    /**
     * @brief Replace the requirements model displayed by the view
     * @param new_model The new requirements model to display in the view
     * @post The off-widget is hidden, and the on-widget is displayed
     */
    void select_model(const Glib::RefPtr<Gio::ListStore<Requirement>>& new_model);

    /**
     * @brief Disconnect the view from its currently displayed model
     * @post The on-widget is hidden, and the off-widget is displayed
     */
    void deselect_model() const;

private:
    Glib::RefPtr<Gio::ListStore<Requirement>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;
    Gtk::Widget * empty_widget;

    static const char * const area_name;

    Gtk::ColumnView * view;

    ContextButtonCorrespondence context_menu;

    void configure_new_requirement_popup(Gtk::Builder& builder) const;

    void configure_edit_requirement_popup(Gtk::Builder& builder) const;

    void configure_delete_requirement_popup(Gtk::Builder& builder) const;

    void configure_duplicate_requirement_popup(Gtk::Builder& builder) const;

    /**
     * @brief Set up a GTK label within the given container
     * @param list_item The container into which the label should be emplaced
     * @param mono_styling Should the label be styled according to the standard monospace style?
     */
    static void on_setup_label(const Glib::RefPtr<Gtk::ListItem> &list_item, bool mono_styling = false);

    /**
     * @brief Extract the graphical label and referenced item stored in a Gtk::ListItem for a requirements table entry
     * @param list_item The representative Gtk::ListItem
     * @return The potentially NULL label and potentially empty Requirement item reference
     */
    static std::pair<Gtk::Label *, Glib::RefPtr<const Requirement>> on_bind_helper(
        const Glib::RefPtr<Gtk::ListItem>& list_item);

    /**
     * @brief Bind a Requirement name attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_name(const Glib::RefPtr<Gtk::ListItem> &list_item);

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

    /**
     * @brief Bind a Requirement creation time attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_created(const Glib::RefPtr<Gtk::ListItem> &list_item);

    /**
     * @brief Bind a Requirement last-modified time attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_modified(const Glib::RefPtr<Gtk::ListItem> &list_item);
};

}

#endif
