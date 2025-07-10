/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Delete Requirement UI popover
 * @author Oliver Dixon
 * @date 2025-07-10
 * @version Development
 */

#ifndef INDEXDELETEREQUIREMENTPOPOVER_HPP
#define INDEXDELETEREQUIREMENTPOPOVER_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/popover.h>
#include <log4cxx/logger.h>

namespace optifol
{

class RequirementsIndexArea;

/**
 * @class IndexDeleteRequirementPopover
 * @brief Manage the <i>Delete Requirement</i> popover for the Requirements Index area
 * @see RequirementsIndexArea
 *
 * @details
 *  The <i>Delete Requirements</i> popover provides controls for deleting an existing requirement from the requirements
 *  index for the currently selected Subsystem. The following GTK elements are expected to be available from the given
 *  Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>delete_requirement_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>delete_requirement_confirm</code></td>
 *          <td>Confirm deletion of the named Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>delete_requirement_cancel</code></td>
 *          <td>Cancels deletion of a Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>delete_requirement_property_name</code></td>
 *          <td>Text entry area for the name of the new Requirement to delete</td>
 *      </tr>
 *  </table>
 */
class IndexDeleteRequirementPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param index_area A mutating reference to the view of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    IndexDeleteRequirementPopover(Gtk::Builder& builder, RequirementsIndexArea& index_area);

private:
    /**
     * @brief Handle a click of the <i>Confirm</i> by attempting to delete the named Requirement.
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

    void show_popover() const;

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    RequirementsIndexArea& index_area;
    Gtk::Popover * const my_popover;
    Gtk::Button * const confirm_button;
    Gtk::Button * const cancel_button;
    Gtk::Entry * const name_entry;
};

} // namespace optifol

#endif // INDEXDELETEREQUIREMENTPOPOVER_HPP
