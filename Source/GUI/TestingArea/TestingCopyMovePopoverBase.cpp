/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Testing and Compliance copying/moving popover base
 * @author Oliver Dixon
 * @date 2025-08-09
 * @version Development
 */

#include "TestingCopyMovePopoverBase.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingCopyMovePopoverBase::popover_name = "Copy/Move Requirement to Test Group Popover";
const log4cxx::LoggerPtr TestingCopyMovePopoverBase::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "CopyMoveRequirement"});

TestingCopyMovePopoverBase::TestingCopyMovePopoverBase(TestingArea &testing_area, Gtk::Popover *my_popover,
        Gtk::Entry *requirement_entry, Gtk::DropDown *new_test_group_dropdown,
        Gtk::Button *const confirm_button, Gtk::Button *const cancel_button) :
    testing_area(testing_area),
    my_popover(my_popover),
    new_test_group_dropdown(new_test_group_dropdown),
    requirement_entry(requirement_entry)
{
    // Set up buttons and self.
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingCopyMovePopoverBase::popover_show));
    confirm_button->signal_clicked().connect(
            sigc::mem_fun(*this, &TestingCopyMovePopoverBase::confirm_button_clicked));
    cancel_button->signal_clicked().connect(
            sigc::mem_fun(*this, &TestingCopyMovePopoverBase::cancel_button_clicked));

    // Set up dropdown factory.
    new_test_group_dropdown->set_factory(
            configure_combo_box_factory(sigc::ptr_fun(&TestGroup::bind_name_to_label)));
}

void TestingCopyMovePopoverBase::popover_show() const noexcept
{
    // Setup the default destination test group dropdown.
    const auto test_group_model = testing_area.get_active_subsystem()->get_test_groups();
    if (new_test_group_dropdown->get_model() != test_group_model)
        new_test_group_dropdown->set_model(test_group_model);
    new_test_group_dropdown->set_selected(0);

    // Get the selected requirement. If a test is selected, use its parent requirement by walking the tree.
    try {
        const auto selected_requirement = testing_area.get_selected_requirement();
        requirement_entry->set_text(selected_requirement->property_name().get_value());
    } catch (const std::runtime_error &exception) {
        popover_logger->error(exception.what());
        my_popover->popdown();
    }
}

void TestingCopyMovePopoverBase::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
}

Glib::RefPtr<Gtk::SignalListItemFactory> TestingCopyMovePopoverBase::configure_combo_box_factory(
        sigc::slot<void(const Glib::RefPtr<Gtk::ListItem> &)> &&bind_function)
{
    const auto factory = Gtk::SignalListItemFactory::create();

    factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
    factory->signal_bind().connect(std::move(bind_function));

    return factory;
}

} // namespace optifol
