/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the <i>Run Tests</i> popover in the <i>Testing and Compliance</i> area.
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#include "TestingRunTestsPopover.hpp"

#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingRunTestsPopover::popover_name = "Run Tests Popover";
const log4cxx::LoggerPtr TestingRunTestsPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "RunTests"});

TestingRunTestsPopover::TestingRunTestsPopover(Gtk::Builder &builder, TestingArea &testing_area) :
    testing_area(testing_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "run_tests_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "run_tests_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "run_tests_cancel")),
    test_group_name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "test_group_name"))
{
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::show_popover));
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::cancel_button_clicked));
}

void TestingRunTestsPopover::confirm_button_clicked() noexcept
{
    // TODO URGENT: verify noexcept property of this and the lambda.
    const auto selected_test_group = testing_area.get_selected_test_group();
    const auto begin = selected_test_group->begin_execution_groups();
    const auto end = selected_test_group->end_execution_groups();

    for (std::remove_const_t<decltype(begin)> exe_group_it = begin; exe_group_it != end; ++exe_group_it)
        exe_group_it->get()->run();
}

void TestingRunTestsPopover::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void TestingRunTestsPopover::clear_inputs() const noexcept
{
    test_group_name_entry->set_text("");
}

void TestingRunTestsPopover::show_popover() const noexcept
{
    try {
        test_group_name_entry->set_text(testing_area.get_selected_test_group()->property_name().get_value());
    } catch (const std::runtime_error& selection_error) {
        popover_logger->error("Could not discover the selected Test Group entry.");
        popover_logger->error(selection_error.what());
    }
}

} // namespace optifol
