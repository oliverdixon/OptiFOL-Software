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

#include <cassert>

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
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "duplicate_requirement_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "duplicate_requirement_cancel")),
    old_name_entry(
            GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "duplicate_requirement_property_old_name")),
    new_name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "duplicate_requirement_property_new_name"))
{
    confirm_button->signal_clicked().connect(
            sigc::mem_fun(*this, &IndexDuplicateRequirementPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(
            sigc::mem_fun(*this, &IndexDuplicateRequirementPopover::cancel_button_clicked));
    my_popover->signal_show().connect(sigc::mem_fun(*this, &IndexDuplicateRequirementPopover::show_popover));
}

void IndexDuplicateRequirementPopover::confirm_button_clicked() const
{
    my_popover->popdown();

    const auto active_subsystem = index_area.observe_active_subsystem();
    assert(active_subsystem != nullptr);

    const auto active_data_model = active_subsystem->requirements;
    assert(active_data_model != nullptr); // TODO remove all asserts with proper error logging

    const auto candidate =
            std::dynamic_pointer_cast<const Requirement>(active_data_model->get_item(index_area.get_selected_index()));

    if (candidate != nullptr)
        // TODO StorageObjectBase needs to define a clone virtual member function.
        index_area.construct_and_add_requirement(
                new_name_entry->get_text(),
                candidate->property_statement().get_value(),
                candidate->property_description().get_value(),
                candidate->property_priority().get_value(),
                candidate->property_test_input().get_value()
            );

    popover_logger->info("Duplicated new subsystem requirement with name \"" + new_name_entry->get_text() +
        "\" from existing \"" + old_name_entry->get_text() + "\".");
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
    index_area.update_with_selected_name(*old_name_entry);
}

} // namespace optifol
