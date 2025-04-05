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

#include "../GTKHelpers.hpp"
#include "../../Storage/Subsystem/SubsystemHierarchicalModel.hpp"

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
     * @param view The GTK requirements view table consisting of columns for each requirement attribute
     * @param widget_off The abstract GTK widget to display when there is no model to display in the view
     * @param widget_on The abstract GTK widget to display when there is a model to display in the view
     */
    explicit RequirementsIndexArea(Gtk::ColumnView * view, Gtk::Widget * widget_off, Gtk::Widget * widget_on);

    /**
     * @brief Replace the requirements model displayed by the view
     * @param new_model The new requirements model to display in the view
     * @post The off-widget is hidden, and the on-widget is displayed
     */
    void select_model(const Glib::RefPtr<RequirementHierarchicalModel>& new_model) const;

    /**
     * @brief Disconnect the view from its currently displayed model
     * @post The on-widget is hidden, and the off-widget is displayed
     */
    void deselect_model() const;

private:
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;

    /**
     * @brief Set up an editable GTK label within the given container
     * @tparam SetterFunc The function signature of the on-edit callback
     * @param list_item The container into which the label should be emplaced
     * @param setter_function The on-edit callback function r-value
     * @param mono_styling Should the label be styled according to the standard monospace style?
     */
    template<typename SetterFunc>
    void on_setup_label(const Glib::RefPtr<Gtk::ListItem> &list_item, SetterFunc&& setter_function,
        bool mono_styling = false);

    /**
     * @brief Helper function to retrieve and cast the labels for binding, independent of the getter function semantics
     * @param list_item The list item containing the label to be bound
     * @return An owning reference to the represented requirement, and a raw pointer to the GTK label widget
     */
    std::pair<Glib::RefPtr<Requirement>, Gtk::EditableLabel*> on_bind_setup(
        const Glib::RefPtr<Gtk::ListItem>& list_item) const;

    /**
     * @brief Bind a non-optional Requirement attribute to a label
     * @tparam GetterFunc The function signature of the Requirement-detail getter
     * @param list_item The container in which the destination label exists
     * @param getter_function The function to retrieve the detailed information from the Requirement represented by the
     *  given list item
     */
    template<typename GetterFunc>
    void on_bind_label(const Glib::RefPtr<Gtk::ListItem>& list_item, GetterFunc&& getter_function) const;

    /**
     * @brief Bind an optional Requirement attribute to a label; absent values are handled correctly
     * @tparam GetterFunc The function signature of the Requirement-detail getter
     * @param list_item The container in which the destination label exists
     * @param getter_function The function to retrieve the detailed (optional) information from the Requirement
     *  represented by the given list item
     */
    template<mp_helpers::OptionalReturner GetterFunc>
    void on_bind_label(const Glib::RefPtr<Gtk::ListItem>& list_item, GetterFunc&& getter_function) const;

    /**
     * @brief Handle a label being updated by re-setting attributes on the corresponding Requirement object
     * @tparam SetterFunc The function signature of the Requirement-detail setter
     * @param list_item The container in which the source label exists
     * @param setter_function The function to set the detailed information in the Requirement represented by the given
     *  list item
     * @warning There are no requirements imposed upon the setter function at compile-time. It may throw exceptions in
     *  the event of an unacceptable incoming value. This is an intentional design decision.
     */
    template<typename SetterFunc>
    void on_edit_label(const Glib::RefPtr<Gtk::ListItem>& list_item, SetterFunc&& setter_function);
};

}

#endif
