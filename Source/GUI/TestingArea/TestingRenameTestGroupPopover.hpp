/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Testing and Compliance <i>Rename Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-10
 * @version Development
 */

#ifndef OPTIFOL_TESTINGRENAMETESTGROUPPOPOVER_HPP
#define OPTIFOL_TESTINGRENAMETESTGROUPPOPOVER_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/popover.h>
#include <log4cxx/logger.h>

namespace optifol
{

class TestingArea;

/**
 * @class TestingRenameTestGroupPopover
 * @brief Manage the <i>Rename Test Group</i> popover for the Testing and Compliance area.
 * @see TestingArea for the parent area
 *
 * @details
 *  The <i>Rename Test Group</i> popover provides controls for renaming an existing TestGroup. The following GTK
 *  elements are expected to be available from the given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>rename_test_group_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>rename_test_group_confirm</code></td>
 *          <td>Confirm deletion button</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>rename_test_group_cancel</code></td>
 *          <td>Cancel deletion button</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>rename_test_group_old_name</code></td>
 *          <td>Read-only entry to hold the name of the existing TestGroup to rename</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>rename_test_group_old_name</code></td>
 *          <td>Entry to accept the new name of the TestGroup</td>
 *      </tr>
 *  </table>
 */
class TestingRenameTestGroupPopover : public sigc::trackable
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param testing_area A mutating reference to the TestingArea of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    TestingRenameTestGroupPopover(Gtk::Builder& builder, TestingArea& testing_area);

private:
    /**
     * @brief Handle a click of the <i>Confirm</i> button by renaming the selected TestGroup.
     */
    void confirm_button_clicked() const noexcept;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by hiding the popover.
     */
    void cancel_button_clicked() const noexcept;

    /**
     * @brief Handle a show of the popover by displaying the selected TestGroup name.
     */
    void popover_shown() const noexcept;

    /**
     * @brief Clear all user-provided inputs.
     */
    void clear_inputs() const noexcept;

    /**
     * @brief React to a change in the @ref new_name_entry contents; in particular, enable or disable the <i>Confirm</i>
     *  button.
     */
    void new_name_changed() const noexcept;

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    TestingArea& testing_area;

    Gtk::Popover * const my_popover;
    Gtk::Button * const confirm_button;
    Gtk::Entry * const old_name_entry;
    Gtk::Entry * const new_name_entry;
};

} // namespace optifol

#endif // OPTIFOL_TESTINGRENAMETESTGROUPPOPOVER_HPP
