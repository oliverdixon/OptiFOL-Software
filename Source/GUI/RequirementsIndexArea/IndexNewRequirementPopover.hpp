/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the New Requirement UI popover
 * @author Oliver Dixon
 * @date 2025-07-10
 * @version Development
 */

#ifndef INDEXNEWREQUIREMENTPOPOVER_HPP
#define INDEXNEWREQUIREMENTPOPOVER_HPP

#include <gtkmm/builder.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/entry.h>
#include <gtkmm/textview.h>

#include "ManageTestsPopover.hpp"

namespace optifol
{

class RequirementsIndexArea;

/**
 * @class IndexNewRequirementPopover
 * @brief Manage the <i>New Requirement</i> popover for the Requirements Index area
 * @see RequirementsIndexArea for the parent area
 *
 * @details
 *  The <i>New Requirement</i> popover provides controls for creating a new Requirement for the currently selected
 *  Subsystem. The following GTK elements are expected to be available from the given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>new_requirement_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>new_requirement_confirm</code></td>
 *          <td>Confirm creation of a new Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>new_requirement_cancel</code></td>
 *          <td>Cancels creation of a new Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>new_requirement_property_name</code></td>
 *          <td>Text entry area for the name of the new Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::TextView</td>
 *          <td><code>new_requirement_property_description</code></td>
 *          <td>Multi-line text entry area for the description of the new Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>new_requirement_property_sentence</code></td>
 *          <td>Text entry area for the FOL statement of the new Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>new_requirement_test_count</code></td>
 *          <td>Read-only area for the test summary of the new Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::DropDown</td>
 *          <td><code>new_requirement_property_priority</code></td>
 *          <td>Text entry area for the optional associated test(s) of the new Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>manage_tests_popover</code></td>
 *          <td>Popover for managing tests</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>new_requirement_manage_tests</code></td>
 *          <td>Button for activating <code>manage_tests_popover</code> popover</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the expected
 *  type instantiations.
 */
class IndexNewRequirementPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param index_area A mutating reference to the view of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    IndexNewRequirementPopover(Gtk::Builder& builder, RequirementsIndexArea& index_area);

private:
    /**
     * @brief Handle a click of the <i>Confirm</i> by attempting to create a Requirement with the given characteristics.
     */
    void confirm_button_clicked();

    /**
     * @brief Handle a click of the <i>Cancel</i> button by discarding all input and closing the popover.
     */
    void cancel_button_clicked() const;

    /**
     * @brief Handle a show of the popover by selecting the correct test specification model for the
     *  @ref manage_tests_popover.
     */
    void popover_shown() noexcept;

    /**
     * @brief Clear all user fields in the popover.
     */
    void clear_inputs();

    /**
     * @brief Handle a new committed Requirement name by enabling the <i>Confirm</i> button.
     */
    void name_entry_changed() const noexcept;

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    RequirementsIndexArea& index_area;

    Gtk::Popover * const my_popover;
    Gtk::Button * const confirm_button;
    Gtk::Entry * const name_entry;
    Gtk::TextView * const description_entry;
    Gtk::Entry * const statement_entry;
    Gtk::Entry * const test_summary;
    Gtk::DropDown * const priority_entry;

    ManageTestsPopover manage_tests_popover;
    Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> test_specification;
};

} // namespace optifol

#endif // INDEXNEWREQUIREMENTPOPOVER_HPP
