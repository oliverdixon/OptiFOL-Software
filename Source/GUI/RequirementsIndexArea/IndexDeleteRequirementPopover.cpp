/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Delete Requirement UI popover
 * @author Oliver Dixon
 * @date 2025-07-10
 * @version Development
 */

#include "IndexDeleteRequirementPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../../Storage/Subsystem.hpp"
#include "RequirementsIndexArea.hpp"

namespace optifol
{

const char *const IndexDeleteRequirementPopover::popover_name = "Delete Requirement Popover";
const log4cxx::LoggerPtr IndexDeleteRequirementPopover::popover_logger =
        Logging::get_logger({"GUI", "RequirementsIndex", "DeleteRequirement"});

IndexDeleteRequirementPopover::IndexDeleteRequirementPopover(Gtk::Builder &builder, RequirementsIndexArea &index_area) :
    index_area(index_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "delete_requirement_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "delete_requirement_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "delete_requirement_cancel")),
    name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "delete_requirement_property_name"))
{
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this,
        &IndexDeleteRequirementPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this,
        &IndexDeleteRequirementPopover::cancel_button_clicked));
    my_popover->signal_show().connect(sigc::mem_fun(*this, &IndexDeleteRequirementPopover::show_popover));
}

void IndexDeleteRequirementPopover::confirm_button_clicked() const
{
    my_popover->popdown();
    const auto slated_requirement = index_area.get_selection();
    index_area.get_active_subsystem()->delete_object(slated_requirement);
    popover_logger->info("Deleted a subsystem requirement with name \"" +
        slated_requirement->property_name().get_value() + "\".");
}

void IndexDeleteRequirementPopover::cancel_button_clicked() const
{
    my_popover->popdown();
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void IndexDeleteRequirementPopover::clear_inputs() const
{
    name_entry->set_text("");
}
void IndexDeleteRequirementPopover::show_popover() const
{
    name_entry->set_text(index_area.get_selection()->property_name().get_value());
}

} // namespace optifol
