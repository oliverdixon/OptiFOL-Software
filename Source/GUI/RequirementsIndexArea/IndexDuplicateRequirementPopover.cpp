/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Duplicate Requirement UI popover
 * @author Oliver Dixon
 * @date 2025-07-10
 * @version Development
 */

#include "IndexDuplicateRequirementPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../Storage/Subsystem.hpp"
#include "RequirementsIndexArea.hpp"

namespace optifol
{

const char *const IndexDuplicateRequirementPopover::popover_name = "Duplicate Requirement Popover";
const log4cxx::LoggerPtr IndexDuplicateRequirementPopover::popover_logger =
        Logging::get_logger({"GUI", "RequirementsIndex", "DuplicateRequirement"});

IndexDuplicateRequirementPopover::IndexDuplicateRequirementPopover(
        Gtk::Builder &builder, RequirementsIndexArea &index_area) :
    index_area(index_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "duplicate_requirement_popover")),
    confirm_button(
            GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "duplicate_requirement_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "duplicate_requirement_cancel")),
    old_name_entry(GTKHelpers::get_widget<Gtk::Entry>(
            popover_name, builder, "duplicate_requirement_property_old_name")),
    new_name_entry(GTKHelpers::get_widget<Gtk::Entry>(
            popover_name, builder, "duplicate_requirement_property_new_name"))
{
    confirm_button->signal_clicked().connect(
            sigc::mem_fun(*this, &IndexDuplicateRequirementPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(
            sigc::mem_fun(*this, &IndexDuplicateRequirementPopover::cancel_button_clicked));
    my_popover->signal_show().connect(sigc::mem_fun(*this, &IndexDuplicateRequirementPopover::show_popover));
}

void IndexDuplicateRequirementPopover::confirm_button_clicked() const noexcept
{
    my_popover->popdown();

    try {
        const auto subsystem = index_area.get_active_subsystem();
        subsystem->duplicate_requirement(*index_area.get_selection());

        popover_logger->info("Duplicated new Subsystem Requirement with name \"" +
                new_name_entry->get_text() + "\" from existing \"" + old_name_entry->get_text() + "\".");
    } catch (const std::runtime_error &error) {
        popover_logger->error("Could not duplicate Subsystem Requirement with name \"" +
                new_name_entry->get_text() + "\".");
        popover_logger->error(error.what());
    }

    clear_inputs();
}

void IndexDuplicateRequirementPopover::cancel_button_clicked() const
{
    my_popover->popdown();
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void IndexDuplicateRequirementPopover::clear_inputs() const
{
    old_name_entry->set_text("");
    new_name_entry->set_text("");
}

void IndexDuplicateRequirementPopover::show_popover() const
{
    old_name_entry->set_text(index_area.get_selection()->property_name().get_value());
}

} // namespace optifol
