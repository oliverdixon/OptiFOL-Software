/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Testing and Compliance copying/moving popover base
 * @author Oliver Dixon
 * @date 2025-08-09
 * @version Development
 */

#ifndef OPTIFOL_TESTINGCOPYMOVEPOPOVERBASE_HPP
#define OPTIFOL_TESTINGCOPYMOVEPOPOVERBASE_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/entry.h>
#include <gtkmm/popover.h>
#include <gtkmm/signallistitemfactory.h>
#include <log4cxx/logger.h>

namespace optifol
{

class TestingArea;
class Requirement;
class TestGroup;

/**
 * @class TestingCopyMovePopoverBase
 * @brief Provides a common base for GTK popovers providing functionality to copy or move Requirement objects between
 *  TestGroup objects. Default callbacks are provided, but inheritors must implement the <i>Confirm</i> action.
 */
class TestingCopyMovePopoverBase
{
public:
    /**
     * @brief Destruct the TestingCopyMovePopoverBase instance.
     */
    virtual ~TestingCopyMovePopoverBase() = default;

protected:
    /**
     * @brief Construct a new TestingCopyMovePopoverBase with the given GUI GTK elements.
     * @param testing_area The parental area.
     * @param my_popover The managed popover.
     * @param requirement_entry The read-only entry for the selected Requirement name.
     * @param new_test_group_dropdown The selection control for the target TestGroup.
     * @param confirm_button The button to confirm the action.
     * @param cancel_button The button to cancel the action.
     */
    TestingCopyMovePopoverBase(TestingArea& testing_area,
        Gtk::Popover * my_popover,
        Gtk::Entry * requirement_entry,
        Gtk::DropDown * new_test_group_dropdown,
        Gtk::Button * confirm_button,
        Gtk::Button * cancel_button);

    /**
     * @brief Display the popover by setting context-sensitive default selections for the drop-downs.
     * @see @ref set_test_group_dropdown for configuration of the <i>Current Test Group</i> drop-down.
     * @see @ref set_new_test_group_dropdown for configuration of the <i>New Test Group</i> drop-down.
     * @see @ref set_requirement_dropdown for configuration of the <i>Requirement Name</i> drop-down.
     */
    void popover_show() const noexcept;

    /**
     * @brief Handle a click of the <i>Confirm</i> button.
     */
    virtual void confirm_button_clicked() const noexcept = 0;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by hiding the popover.
     */
    void cancel_button_clicked() const noexcept;

    /**
     * @brief Copies the ref-counted pointer to the selected TestGroup. If a Requirement or Test is selected, the owning
     *  TestGroup is returned.
     * @return The owning TestGroup of the selected object.
     * @throws std::runtime_error There is no selected object.
     */
    Glib::RefPtr<TestGroup> get_selected_test_group() const;

    /**
     * @brief Copies the ref-counted pointer to the selected Requirement. If a Test is selected, the owning Requirement
     *  is returned.
     * @return The owning Requirement of the selected Test.
     * @throws std::runtime_error There is no selected object, or the selected object is unsuitable.
     */
    Glib::RefPtr<Requirement> get_selected_requirement() const;

    /**
     * @brief Creates, configures, and returns a GTK factory for a Gtk::DropDown with a given bind function.
     * @param bind_function The handler for setting the contents of the Gtk::DropDown element for the given
     *  Gtk::ListItem.
     * @return The configured factory.
     */
    static Glib::RefPtr<Gtk::SignalListItemFactory> configure_combo_box_factory(
        sigc::slot<void(const Glib::RefPtr<Gtk::ListItem> &)> &&bind_function);

    TestingArea& testing_area;
    Gtk::Popover * const my_popover;
    Gtk::DropDown * const new_test_group_dropdown;

private:
    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    Gtk::Entry * const requirement_entry;
};

} // namespace optifol

#endif // OPTIFOL_TESTINGCOPYMOVEPOPOVERBASE_HPP
