/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Testing and Compliance <i>Copy Requirement to Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#ifndef OPTIFOL_TESTINGCOPYTOTESTGROUPPOPOVER_HPP
#define OPTIFOL_TESTINGCOPYTOTESTGROUPPOPOVER_HPP

#include <gtkmm/builder.h>

#include "TestingCopyMovePopoverBase.hpp"

namespace optifol
{

class TestingArea;

/**
 * @class TestingCopyToTestGroupPopover
 * @brief Manage the <i>Copy Requirement to Test Group</i> popover for the TestingArea.
 * @see TestingArea for the parent area.
 *
 * @details
 *  The <i>Copy Requirement to Test Group</i> popover provides controls for copying an existing Requirement in
 * a TestGroup to another existing TestGroup. The control automatically populates its entries with likely
 * defaults using the TestGroup/Requirement/Test models from the parental TestArea. The following GTK elements
 * are expected from the given Gtk::Builder: <table> <tr> <th>GTK C++ Class</th> <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>copy_to_test_group_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>copy_to_test_group_confirm</code></td>
 *          <td>Confirm copy of Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>copy_to_test_group_cancel</code></td>
 *          <td>Cancels copy of Requirement</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>copy_to_test_group_requirement</code></td>
 *          <td>Reported name of Requirement to copy</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::DropDown</td>
 *          <td><code>copy_to_test_group_new_group</code></td>
 *          <td>Selection of destination TestGroup/move</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the
 * expected type instantiations.
 */
class TestingCopyToTestGroupPopover : public TestingCopyMovePopoverBase
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param testing_area A mutating reference to the TestingArea of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    TestingCopyToTestGroupPopover(Gtk::Builder &builder, TestingArea &testing_area);

private:
    /**
     * @copybrief TestingCopyMovePopoverBase::confirm_button_clicked
     * @details Copies the Requirement from its current TestGroup to the selected destination TestGroup. The
     * source TestGroup retains its copy of the Requirement.
     */
    void confirm_button_clicked() const noexcept override;

    static const char *const popover_name;
    static const log4cxx::LoggerPtr popover_logger;
};

} // namespace optifol

#endif // OPTIFOL_TESTINGCOPYTOTESTGROUPPOPOVER_HPP
