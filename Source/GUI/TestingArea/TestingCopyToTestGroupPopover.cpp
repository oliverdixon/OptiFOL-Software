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
#include "../Logging.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingCopyToTestGroupPopover::popover_name = "Copy Requirement to Test Group Popover";
const log4cxx::LoggerPtr TestingCopyToTestGroupPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "CopyRequirement"});

TestingCopyToTestGroupPopover::TestingCopyToTestGroupPopover(Gtk::Builder &builder, TestingArea &testing_area) :
    testing_area(testing_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "copy_to_test_group_popover")),
    current_test_group_dropdown(GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder,
        "copy_to_test_group_current_group")),
    requirement_dropdown(GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder,
        "copy_to_test_group_requirement")),
    new_test_group_dropdown(GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder,
        "copy_to_test_group_new_group"))
{
    // Get extra elements needed only for the constructor lifetime.
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(popover_name, builder,
        "copy_to_test_group_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "copy_to_test_group_cancel");

    // Set up buttons and self.
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingCopyToTestGroupPopover::popover_show));
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this,
        &TestingCopyToTestGroupPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this,
        &TestingCopyToTestGroupPopover::cancel_button_clicked));

    // TODO: Set up dropdown synchronisation such that the Requirement is restricted by the selected TestGroup.

    // Set up dropdown factories.
    current_test_group_dropdown->set_factory(configure_combo_box_factory(sigc::ptr_fun(&TestGroup::bind_name_to_label)));
    new_test_group_dropdown->set_factory(configure_combo_box_factory(sigc::ptr_fun(&TestGroup::bind_name_to_label)));
    requirement_dropdown->set_factory(configure_combo_box_factory(sigc::ptr_fun(&TestGroup::bind_name_to_label)));
}

void TestingCopyToTestGroupPopover::popover_show() const
{
    // TODO: what if a Requirement or Test is selected? We still need to find the appropriate TestGroup.

    current_test_group_dropdown->set_model(testing_area.get_active_subsystem()->get_test_groups());
    current_test_group_dropdown->set_selected(testing_area.get_selection_index());
}

void TestingCopyToTestGroupPopover::confirm_button_clicked() const noexcept
{

}

void TestingCopyToTestGroupPopover::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
}

Glib::RefPtr<Gtk::SignalListItemFactory> TestingCopyToTestGroupPopover::configure_combo_box_factory(
        sigc::slot<void(const Glib::RefPtr<Gtk::ListItem> &)> &&bind_function)
{
    const auto factory = Gtk::SignalListItemFactory::create();

    factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
    factory->signal_bind().connect(std::move(bind_function));

    return factory;
}

} // namespace optifol
