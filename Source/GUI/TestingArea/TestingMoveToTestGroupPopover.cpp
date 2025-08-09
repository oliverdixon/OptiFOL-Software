/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Testing and Compliance <i>Move Requirement to Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-09
 * @version Development
 */

#include "TestingMoveToTestGroupPopover.hpp"
#include "../GTKHelpers.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingMoveToTestGroupPopover::popover_name = "Move Requirement to Test Group Popover";
const log4cxx::LoggerPtr TestingMoveToTestGroupPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "CopyMoveRequirement", "Move"});

TestingMoveToTestGroupPopover::TestingMoveToTestGroupPopover(Gtk::Builder &builder, TestingArea &testing_area) :
    TestingCopyMovePopoverBase(
        testing_area,
        GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "move_to_test_group_popover"),
        GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "move_to_test_group_requirement"),
        GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder, "move_to_test_group_new_group"),
        GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "move_to_test_group_confirm"),
        GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "move_to_test_group_cancel")
    )
{
}

void TestingMoveToTestGroupPopover::confirm_button_clicked() const noexcept
{
    my_popover->popdown();

    try {
        const auto source_test_group = get_selected_test_group();
        const auto requirement = get_selected_requirement();
        const auto target_test_group = dynamic_cast<TestGroup *>(new_test_group_dropdown->get_selected_item().get());

        if (source_test_group == nullptr) {
            popover_logger->warn("Not moving Requirement, as no source Test Group selected.");
            return;
        }

        if (requirement == nullptr) {
            popover_logger->warn("Not moving Requirement, as no Requirement selected.");
            return;
        }

        if (target_test_group == nullptr) {
            popover_logger->warn("Not moving Requirement, as no destination Test Group selected.");
            return;
        }

        source_test_group->delete_object(requirement);
        target_test_group->insert_object(requirement);

        popover_logger->debug("Moved Requirement \"" + requirement->property_name().get_value() +
            "\" from Test Group \"" + source_test_group->property_name().get_value() + "\" into Test Group \"" +
            target_test_group->property_name().get_value() + "\".");
    } catch (const std::runtime_error& exception) {
        popover_logger->error("Not moving Requirement: " + std::string(exception.what()));
    }
}

} // namespace optifol
