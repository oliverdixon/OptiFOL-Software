/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Testing and Compliance <i>Copy Requirement to Test Group</i> popover
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#include "TestingCopyToTestGroupPopover.hpp"
#include "../GTKHelpers.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingCopyToTestGroupPopover::popover_name = "Copy Requirement to Test Group Popover";
const log4cxx::LoggerPtr TestingCopyToTestGroupPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "CopyMoveRequirement", "Copy"});

TestingCopyToTestGroupPopover::TestingCopyToTestGroupPopover(Gtk::Builder &builder, TestingArea &testing_area) :
    TestingCopyMovePopoverBase(
        testing_area,
        GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "copy_to_test_group_popover"),
        GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "copy_to_test_group_requirement"),
        GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder, "copy_to_test_group_new_group"),
        GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "copy_to_test_group_confirm"),
        GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "copy_to_test_group_cancel")
    )
{
}

void TestingCopyToTestGroupPopover::confirm_button_clicked() const noexcept
{
    my_popover->popdown();

    try {
        const auto requirement = get_selected_requirement();
        const auto target_test_group = dynamic_cast<TestGroup *>(new_test_group_dropdown->get_selected_item().get());

        if (requirement == nullptr) {
            popover_logger->warn("Not copying Requirement, as no Requirement selected.");
            return;
        }

        if (target_test_group == nullptr) {
            popover_logger->warn("Not copying Requirement, as no destination Test Group selected.");
            return;
        }

        target_test_group->insert_object(requirement);
        popover_logger->debug("Copied Requirement \"" + requirement->property_name().get_value() +
            "\" into Test Group \"" + target_test_group->property_name().get_value() + "\".");
    } catch (const std::runtime_error& exception) {
        popover_logger->error("Not copying Requirement: " + std::string(exception.what()));
    }
}


} // namespace optifol
