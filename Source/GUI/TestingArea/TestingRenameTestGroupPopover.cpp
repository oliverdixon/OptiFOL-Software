/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Testing and Compliance <i>Rename Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-10
 * @version Development
 */

#include "TestingRenameTestGroupPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingRenameTestGroupPopover::popover_name = "Rename Test Group Popover";
const log4cxx::LoggerPtr TestingRenameTestGroupPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "RenameTestGroup"});

TestingRenameTestGroupPopover::TestingRenameTestGroupPopover(
        Gtk::Builder &builder, TestingArea &testing_area) :
    testing_area(testing_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "rename_test_group_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "rename_test_group_confirm")),
    old_name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "rename_test_group_old_name")),
    new_name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "rename_test_group_new_name"))
{
    // Get extra elements needed only for the constructor lifetime.
    const auto cancel_button =
            GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "rename_test_group_cancel");

    // Set up buttons and self.
    confirm_button->signal_clicked().connect(
            sigc::mem_fun(*this, &TestingRenameTestGroupPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(
            sigc::mem_fun(*this, &TestingRenameTestGroupPopover::cancel_button_clicked));
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingRenameTestGroupPopover::popover_shown));
    new_name_entry->signal_changed().connect(
            sigc::mem_fun(*this, &TestingRenameTestGroupPopover::new_name_changed));
}

void TestingRenameTestGroupPopover::confirm_button_clicked() const noexcept
{
    try {
        const auto existing_group = testing_area.get_selected_test_group();
        existing_group->property_name().set_value(new_name_entry->get_text());
        popover_logger->debug("Renamed Test Group \"" + old_name_entry->get_text() + "\" to \"" +
                existing_group->property_name().get_value() + "\".");
    } catch (const std::runtime_error &selection_error) {
        popover_logger->error("Could not discover the selected Test Group entry.");
        popover_logger->error(selection_error.what());
    }

    my_popover->popdown();
    clear_inputs();
}

void TestingRenameTestGroupPopover::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
    clear_inputs();
}

void TestingRenameTestGroupPopover::popover_shown() const noexcept
{
    try {
        old_name_entry->set_text(testing_area.get_selected_test_group()->property_name().get_value());
    } catch (const std::runtime_error &selection_error) {
        my_popover->popdown();
        popover_logger->error("Could not discover the selected Test Group entry.");
        popover_logger->error(selection_error.what());
    }
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive. Invoked from popover callbacks.
void TestingRenameTestGroupPopover::clear_inputs() const noexcept
{
    new_name_entry->set_text("");
}

void TestingRenameTestGroupPopover::new_name_changed() const noexcept
{
    const bool should_enable = new_name_entry->get_text_length() > 0;
    if (new_name_entry->get_sensitive() != should_enable)
        new_name_entry->set_sensitive(should_enable);
}

} // namespace optifol
