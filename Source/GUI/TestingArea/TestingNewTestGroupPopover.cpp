/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Testing and Compliance <i>New Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#include "TestingNewTestGroupPopover.hpp"

#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingNewTestGroupPopover::popover_name = "New Test Group Popover";
const log4cxx::LoggerPtr TestingNewTestGroupPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "NewTestGroup"});

TestingNewTestGroupPopover::TestingNewTestGroupPopover(Gtk::Builder &builder, TestingArea &testing_area) :
    testing_area(testing_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "new_test_group_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "new_test_group_confirm")),
    name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "new_test_group_name"))
{
    // Get extra elements needed only for the constructor lifetime.
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "new_test_group_cancel");

    // Set up buttons and self.
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingNewTestGroupPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingNewTestGroupPopover::cancel_button_clicked));
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingNewTestGroupPopover::popover_shown));
    name_entry->signal_changed().connect(sigc::mem_fun(*this, &TestingNewTestGroupPopover::name_entry_changed));
}

void TestingNewTestGroupPopover::confirm_button_clicked() const
{
    my_popover->popdown();
    testing_area.observe_active_subsystem()->get_test_groups()->append(Glib::make_refptr_for_instance(
        new TestGroup(name_entry->get_text())));

    popover_logger->debug("Created new Test Group with name \"" + name_entry->get_text() + "\".");
}

void TestingNewTestGroupPopover::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
}

void TestingNewTestGroupPopover::popover_shown() const noexcept
{
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from popover show callback.
void TestingNewTestGroupPopover::clear_inputs() const noexcept
{
    confirm_button->set_sensitive(false);
    name_entry->set_text("");
}

void TestingNewTestGroupPopover::name_entry_changed() const noexcept
{
    confirm_button->set_sensitive(!name_entry->get_text().empty());
}

} // namespace optifol
