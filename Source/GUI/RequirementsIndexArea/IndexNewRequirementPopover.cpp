/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the New Requirement UI popover
 * @author Oliver Dixon
 * @date 2025-07-10
 * @version Development
 */

#include "IndexNewRequirementPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../../Storage/Subsystem.hpp"
#include "RequirementsIndexArea.hpp"

namespace optifol
{

const char *const IndexNewRequirementPopover::popover_name = "New Requirement Popover";
const log4cxx::LoggerPtr IndexNewRequirementPopover::popover_logger =
        Logging::get_logger({"GUI", "RequirementsIndex", "NewRequirement"});

IndexNewRequirementPopover::IndexNewRequirementPopover(Gtk::Builder &builder, RequirementsIndexArea &index_area) :
    index_area(index_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "new_requirement_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "new_requirement_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "new_requirement_cancel")),
    name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "new_requirement_property_name")),
    description_entry(
            GTKHelpers::get_widget<Gtk::TextView>(popover_name, builder, "new_requirement_property_description")),
    statement_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "new_requirement_property_sentence")),
    priority_entry(GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder, "new_requirement_property_priority")),
    manage_tests_popover(builder),
    edit_tests_button(GTKHelpers::get_widget<Gtk::MenuButton>(popover_name, builder, "new_requirement_manage_tests")),
    edit_tests_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "manage_tests_popover"))
{
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &IndexNewRequirementPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &IndexNewRequirementPopover::cancel_button_clicked));
    edit_tests_button->set_popover(*edit_tests_popover);
}

void IndexNewRequirementPopover::confirm_button_clicked() const
{
    my_popover->popdown();
    index_area.observe_active_subsystem()->build_requirement(
        name_entry->get_text(),
        statement_entry->get_text(),
        description_entry->get_buffer()->get_text(),
        priority_entry->get_selected(),
        manage_tests_popover.get_aggregate_tests()
    );

    popover_logger->info("Created new subsystem requirement with name \"" + name_entry->get_text() + "\".");
    clear_inputs();
}

void IndexNewRequirementPopover::cancel_button_clicked() const
{
    my_popover->popdown();
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void IndexNewRequirementPopover::clear_inputs() const
{
    name_entry->set_text("");
    description_entry->get_buffer()->set_text("");
    statement_entry->set_text("");
    priority_entry->set_selected(0);
}

} // namespace optifol
