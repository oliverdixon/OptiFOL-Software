/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Testing and Compliance <i>New Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#ifndef OPTIFOL_TESTINGNEWTESTGROUPPOPOVER_HPP
#define OPTIFOL_TESTINGNEWTESTGROUPPOPOVER_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/popover.h>
#include <log4cxx/logger.h>

namespace optifol
{

class TestingArea;

/**
 * @class TestingNewTestGroupPopover
 * @brief Manage the <i>New Test Group</i> popover for the Testing and Compliance area.
 * @see TestingArea for the parent area
 *
 * @details
 *  The <i>New Test Group</i> popover provides controls for creating a new TestGroup to hold Requirement
 * objects, which in turn detain one or more Test entities. The following GTK elements are expected to be
 * available from the given Gtk::Builder: <table> <tr> <th>GTK C++ Class</th> <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>new_test_group_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>new_test_group_confirm</code></td>
 *          <td>Confirm changes button</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>new_test_group_cancel</code></td>
 *          <td>Cancel changes button</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>new_test_group_name</code></td>
 *          <td>Entry for the name of the new TestGroup</td>
 *      </tr>
 *  </table>
 */
class TestingNewTestGroupPopover : public sigc::trackable
{
public:
    TestingNewTestGroupPopover(Gtk::Builder &builder, TestingArea &testing_area);

private:
    void confirm_button_clicked() const;

    void cancel_button_clicked() const noexcept;

    void popover_shown() const noexcept;

    void clear_inputs() const noexcept;

    void name_entry_changed() const noexcept;

    static const char *const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    TestingArea &testing_area;

    Gtk::Popover *const my_popover;
    Gtk::Button *const confirm_button;
    Gtk::Entry *const name_entry;
};

} // namespace optifol

#endif // OPTIFOL_TESTINGNEWTESTGROUPPOPOVER_HPP
