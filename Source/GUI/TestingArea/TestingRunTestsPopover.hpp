/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the <i>Run Tests</i> popover in the <i>Testing and Compliance</i> area.
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#ifndef TESTINGRUNTESTSPOPOVER_HPP
#define TESTINGRUNTESTSPOPOVER_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/popover.h>
#include <gtkmm/textview.h>
#include <log4cxx/logger.h>

namespace optifol
{

class TestGroup;
class TestListenerBase;
class TestingArea;

/**
 * @class TestingRunTestsPopover
 * @brief Manage the <i>Run Tests</i> popover for the <i>Testing and Compliance</i> area.
 * @see TestingArea for the parent area
 *
 * @details
 *  The <i>Run Tests</i> popover provides controls for executing unit test specifications through an external
 *  executable(s) for the selected TestGroup within the Subsystem. The following GTK elements are expected to be
 *  available from the given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>run_tests_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>run_tests_confirm</code></td>
 *          <td>Confirm execution of test executable</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>run_tests_cancel</code></td>
 *          <td>Cancels execution of test executables</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>test_group_name</code></td>
 *          <td>Read-only text area for the name of the selected TestGroup</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the expected
 *  type instantiations.
 */
class TestingRunTestsPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param testing_area A mutating reference to the TestingArea of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    TestingRunTestsPopover(Gtk::Builder &builder, TestingArea &testing_area);

private:
    /**
     * @brief Handle a click of the <i>Confirm</i> by attempting to create a Requirement with the given characteristics.
     */
    void confirm_button_clicked() noexcept;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by discarding all input and closing the popover.
     */
    void cancel_button_clicked() const noexcept;

    /**
     * @brief Clear all user fields in the popover
     */
    void clear_inputs() const noexcept;

    /**
     * @brief Handle the popover becoming visible.
     */
    void show_popover() const noexcept;

    static const char *const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    TestingArea &testing_area;

    Gtk::Popover *const my_popover;
    Gtk::Button *const confirm_button;
    Gtk::Button *const cancel_button;
    Gtk::Entry *const test_group_name_entry;
};

} // namespace optifol

#endif // TESTINGRUNTESTSPOPOVER_HPP
