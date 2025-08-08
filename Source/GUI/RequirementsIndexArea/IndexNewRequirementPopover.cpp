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
    name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "new_requirement_property_name")),
    description_entry(
            GTKHelpers::get_widget<Gtk::TextView>(popover_name, builder, "new_requirement_property_description")),
    statement_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "new_requirement_property_sentence")),
    test_summary(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "new_requirement_test_count")),
    priority_entry(GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder, "new_requirement_property_priority")),
    manage_tests_popover(builder)
{
    // Get extra elements needed only for the constructor lifetime.
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "new_requirement_cancel");

    // Set up the test management popover.
    const auto test_button = GTKHelpers::get_widget<Gtk::MenuButton>(popover_name, builder, "new_requirement_manage_tests");
    const auto test_manager = GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "manage_tests_popover");
    test_button->set_popover(*test_manager);

    // Set up buttons and self.
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &IndexNewRequirementPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &IndexNewRequirementPopover::cancel_button_clicked));
    my_popover->signal_show().connect(sigc::mem_fun(*this, &IndexNewRequirementPopover::popover_shown));
    name_entry->signal_changed().connect(sigc::mem_fun(*this, &IndexNewRequirementPopover::name_entry_changed));

    // Force the popover into a known baseline state.
    clear_inputs();
}

void IndexNewRequirementPopover::confirm_button_clicked()
{
    my_popover->popdown();

    index_area.get_active_subsystem()->build_requirement(
        name_entry->get_text(),
        statement_entry->get_text(),
        description_entry->get_buffer()->get_text(),
        priority_entry->get_selected(),
        std::move(test_specification)
    );

    popover_logger->info("Created new subsystem requirement with name \"" + name_entry->get_text() + "\".");
}

void IndexNewRequirementPopover::cancel_button_clicked() const
{
    my_popover->popdown();
}

void IndexNewRequirementPopover::popover_shown() noexcept
{
    clear_inputs(); // Clear down the inputs for the new entry.
    manage_tests_popover.set_model(test_specification);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void IndexNewRequirementPopover::clear_inputs()
{
    name_entry->set_text("");
    description_entry->get_buffer()->set_text("");
    statement_entry->set_text("");
    priority_entry->set_selected(0);
    test_summary->set_text("");
    test_specification = Gio::ListStore<TestSpecificationEntry>::create();
    confirm_button->set_sensitive(false);

    test_specification->signal_items_changed().connect([this](guint, guint, guint) noexcept
    {
        test_summary->set_text(ManageTestsPopover::format_test_summary(*test_specification));
    });
}

void IndexNewRequirementPopover::name_entry_changed() const noexcept
{
    confirm_button->set_sensitive(!name_entry->get_text().empty());
}

} // namespace optifol
