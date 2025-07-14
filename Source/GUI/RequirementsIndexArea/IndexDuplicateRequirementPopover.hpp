/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Duplicate Requirement UI popover
 * @author Oliver Dixon
 * @date 2025-07-10
 * @version Development
 */

#ifndef INDEXDUPLICATEREQUIREMENTPOPOVER_HPP
#define INDEXDUPLICATEREQUIREMENTPOPOVER_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/popover.h>
#include <log4cxx/logger.h>

namespace optifol
{

class RequirementsIndexArea;

/**
 * @class IndexDuplicateRequirementPopover
 * @brief Manage the <i>Duplicate Requirement</i> popover for the Requirements Index area
 * @see RequirementsIndexArea
 *
 * @details
 *  The <i>Duplicate Requirements</i> popover provides controls for duplicating an existing Requirement from the
 *  requirements index for the currently selected Subsystem. By default, the duplicated Requirement copies all metadata
 *  from the existing Requirement, but can be edited separately. The following GTK elements are expected to be available
 *  from the given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>duplicate_requirement_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>duplicate_requirement_confirm</code></td>
 *          <td>Confirm duplication of the named Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>duplicate_requirement_cancel</code></td>
 *          <td>Cancels duplication of a Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>duplicate_requirement_property_old_name</code></td>
 *          <td>Read-only text entry area for the name of the existing Requirement to duplicate</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>duplicate_requirement_property_new_name</code></td>
 *          <td>Text entry area for the name of the new Requirement</td>
 *      </tr>
 *  </table>
 */

class IndexDuplicateRequirementPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param index_area A mutating reference to the view of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    IndexDuplicateRequirementPopover(Gtk::Builder& builder, RequirementsIndexArea& index_area);

private:
    /**
     * @brief Handle a click of the <i>Confirm</i> by attempting to duplicate the named Requirement.
     */
    void confirm_button_clicked() const;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by discarding all input and closing the popover.
     */
    void cancel_button_clicked() const;

    /**
     * @brief Clear all user fields in the popover
     */
    void clear_inputs() const;

    /**
     * @brief Populate all fields given the parental area's selection context and show the popover.
     */
    void show_popover() const;

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    RequirementsIndexArea& index_area;
    Gtk::Popover * const my_popover;
    Gtk::Button * const confirm_button;
    Gtk::Button * const cancel_button;
    Gtk::Entry * const old_name_entry;
    Gtk::Entry * const new_name_entry;
};

} // namespace optifol

#endif // INDEXDUPLICATEREQUIREMENTPOPOVER_HPP
