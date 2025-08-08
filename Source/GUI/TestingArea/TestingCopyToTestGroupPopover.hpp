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
 *
 */
class TestingCopyToTestGroupPopover
{
public:
    TestingCopyToTestGroupPopover(Gtk::Builder& builder, TestingArea& testing_area);

private:
    void popover_show() const;

    void confirm_button_clicked() const noexcept;

    void cancel_button_clicked() const noexcept;

    static Glib::RefPtr<Gtk::SignalListItemFactory> configure_combo_box_factory(
        sigc::slot<void(const Glib::RefPtr<Gtk::ListItem> &)> &&bind_function);

    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    TestingArea& testing_area;

    Gtk::Popover * const my_popover;
    Gtk::DropDown * const current_test_group_dropdown;
    Gtk::DropDown * const requirement_dropdown;
    Gtk::DropDown * const new_test_group_dropdown;
};

} // namespace optifol

#endif // OPTIFOL_TESTINGCOPYTOTESTGROUPPOPOVER_HPP
