/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Testing and Compliance <i>Delete Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-09
 * @version Development
 */

#include "TestingDeleteTestGroupPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingDeleteTestGroupPopover::popover_name = "Delete Test Group Popover";
const log4cxx::LoggerPtr TestingDeleteTestGroupPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "DeleteTestGroup"});

TestingDeleteTestGroupPopover::TestingDeleteTestGroupPopover(
        Gtk::Builder &builder, TestingArea &testing_area) :
    testing_area(testing_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "delete_test_group_popover")),
    test_group_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "delete_test_group_name"))
{
    // Get extra elements needed only for the constructor lifetime.
    const auto confirm_button =
            GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "delete_test_group_confirm");
    const auto cancel_button =
            GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "delete_test_group_cancel");

    // Set up buttons and self.
    confirm_button->signal_clicked().connect(
            sigc::mem_fun(*this, &TestingDeleteTestGroupPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(
            sigc::mem_fun(*this, &TestingDeleteTestGroupPopover::cancel_button_clicked));
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingDeleteTestGroupPopover::popover_shown));
}

void TestingDeleteTestGroupPopover::confirm_button_clicked() const noexcept
{
    try {
        const auto target_group = testing_area.get_selected_test_group();
        const auto groups = testing_area.get_active_subsystem()->get_test_groups().get();
        const auto group_count = groups->get_n_items();

        /*
         * Linear search is acceptable; we maintain a relatively small number of test groups, and it's not
         * worth the additional overhead of maintaining an index hash table.
         */
        for (guint group_index = 0; group_index < group_count; ++group_index)
            if (groups->get_item(group_index) == target_group) {
                groups->remove(group_index);
                popover_logger->debug(
                        "Removed Test Group \"" + target_group->property_name().get_value() + "\".");
                break;
            }
    } catch (const std::runtime_error &selection_error) {
        popover_logger->error("Could not discover the selected Test Group entry.");
        popover_logger->error(selection_error.what());
    }

    my_popover->popdown();
}

void TestingDeleteTestGroupPopover::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
}

void TestingDeleteTestGroupPopover::popover_shown() const noexcept
{
    try {
        test_group_entry->set_text(testing_area.get_selected_test_group()->property_name().get_value());
    } catch (const std::runtime_error &selection_error) {
        my_popover->popdown();
        popover_logger->error("Could not discover the selected Test Group entry.");
        popover_logger->error(selection_error.what());
    }
}

} // namespace optifol
