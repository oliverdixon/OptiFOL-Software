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
#include <gtkmm/dropdown.h>
#include <gtkmm/popover.h>
#include <gtkmm/signallistitemfactory.h>
#include <log4cxx/logger.h>

namespace optifol
{

class TestingArea;

/**
 * @class TestingCopyToTestGroupPopover
 * @brief Manage the <i>Copy Requirement to Test Group</i> popover for the TestingArea.
 * @see TestingArea for the parent area.
 *
 * @details
 *  The <i>Copy Requirement to Test Group</i> popover provides controls for copying an existing Requirement in a
 *  TestGroup to another existing TestGroup. The control automatically populates its entries with likely defaults using
 *  the TestGroup/Requirement/Test models from the parental TestArea. The following GTK elements are expected from the
 *  given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
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
 *          <td>Gtk::DropDown</td>
 *          <td><code>copy_to_test_group_current_group</code></td>
 *          <td>Selection of source Test Group</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::DropDown</td>
 *          <td><code>copy_to_test_group_requirement</code></td>
 *          <td>Selection of Requirement to copy/move</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::DropDown</td>
 *          <td><code>copy_to_test_group_new_group</code></td>
 *          <td>Selection of destination TestGroup/move</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the expected
 *  type instantiations.

 */
class TestingCopyToTestGroupPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param testing_area A mutating reference to the TestingArea of which the popover is a member
     * @param copy_requirements Should the dialog manage the copying or moving of Requirements? If this is unset,
     *  copying a Requirement will be an effective move, removing it from the original Test Group group.
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    TestingCopyToTestGroupPopover(Gtk::Builder& builder, TestingArea& testing_area, bool copy_requirements = true);

private:
    /**
     * @brief Display the popover by setting context-sensitive default selections for the drop-downs.
     * @see @ref set_test_group_dropdown for configuration of the <i>Current Test Group</i> drop-down.
     * @see @ref set_new_test_group_dropdown for configuration of the <i>New Test Group</i> drop-down.
     * @see @ref set_requirement_dropdown for configuration of the <i>Requirement Name</i> drop-down.
     */
    void popover_show() const;

    /**
     * @brief Handle a click of the <i>Confirm</i> button by copying the Requirement.
     */
    void confirm_button_clicked() const noexcept;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by hiding the popover.
     */
    void cancel_button_clicked() const noexcept;

    /**
     * @brief Set the @ref current_test_group_dropdown to the currently selected TestGroup; if a Requirement or Test is
     *  selected, the owning TestGroup is used.
     */
    void set_test_group_dropdown() const;

    /**
     * @brief Set the @ref new_test_group_dropdown to the first available TestGroup of the Subsystem.
     */
    void set_new_test_group_dropdown() const noexcept;

    /**
     * @brief Set the @ref requirement_dropdown to the first available Requirement of the selected TestGroup i.a.w.
     *  @ref set_test_group_dropdown.
     */
    void set_requirement_dropdown() const;

    /**
     * @brief Creates, configures, and returns a GTK factory for a Gtk::DropDown with a given bind function.
     * @param bind_function The handler for setting the contents of the Gtk::DropDown element for the given
     *  Gtk::ListItem.
     * @return The configured factory.
     */
    static Glib::RefPtr<Gtk::SignalListItemFactory> configure_combo_box_factory(
        sigc::slot<void(const Glib::RefPtr<Gtk::ListItem> &)> &&bind_function);

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    TestingArea& testing_area;
    const bool copy_requirements;

    Gtk::Popover * const my_popover;
    Gtk::DropDown * const current_test_group_dropdown;
    Gtk::DropDown * const requirement_dropdown;
    Gtk::DropDown * const new_test_group_dropdown;
};

} // namespace optifol

#endif // OPTIFOL_TESTINGCOPYTOTESTGROUPPOPOVER_HPP
