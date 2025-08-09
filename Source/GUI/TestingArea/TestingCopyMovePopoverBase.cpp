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

TestingCopyMovePopoverBase::TestingCopyMovePopoverBase(
        TestingArea &testing_area,
        Gtk::Popover * my_popover,
        Gtk::Entry *requirement_entry,
        Gtk::DropDown *new_test_group_dropdown,
        Gtk::Button * const confirm_button,
        Gtk::Button * const cancel_button) :
    testing_area(testing_area),
    my_popover(my_popover),
    new_test_group_dropdown(new_test_group_dropdown),
    requirement_entry(requirement_entry)
{
    // Set up buttons and self.
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingCopyMovePopoverBase::popover_show));
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingCopyMovePopoverBase::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingCopyMovePopoverBase::cancel_button_clicked));

    // Set up dropdown factory.
    new_test_group_dropdown->set_factory(configure_combo_box_factory(sigc::ptr_fun(&TestGroup::bind_name_to_label)));
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
        const auto selected_requirement = get_selected_requirement();
        requirement_entry->set_text(selected_requirement->property_name().get_value());
    } catch (const std::runtime_error& exception) {
        popover_logger->error(exception.what());
        my_popover->popdown();
    }
}

void TestingCopyMovePopoverBase::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
}

Glib::RefPtr<TestGroup> TestingCopyMovePopoverBase::get_selected_test_group() const
{
    auto selected_row = testing_area.get_selected_row();
    if (selected_row == nullptr)
        throw std::runtime_error("Popover was made available despite no suitable Test Group selection.");

    while (selected_row->get_depth() > 0)
        selected_row = selected_row->get_parent();

    const auto test_group = std::dynamic_pointer_cast<TestGroup>(selected_row->get_item());
    if (test_group == nullptr)
        throw std::runtime_error("Popover could not find a suitable Test Group.");

    return test_group;
}

Glib::RefPtr<Requirement> TestingCopyMovePopoverBase::get_selected_requirement() const
{
    auto selected_row = testing_area.get_selected_row();
    if (selected_row == nullptr || selected_row->get_depth() == 0)
        throw std::runtime_error("Popover was made available despite no suitable Requirement selection.");

    while (selected_row->get_depth() > 1)
        selected_row = selected_row->get_parent();

    const auto requirement = std::dynamic_pointer_cast<Requirement>(selected_row->get_item());
    if (requirement == nullptr)
        throw std::runtime_error("Popover could not find a suitable Requirement.");

    return requirement;
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
