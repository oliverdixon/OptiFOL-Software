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

TestingCopyToTestGroupPopover::TestingCopyToTestGroupPopover(Gtk::Builder &builder, TestingArea &testing_area,
        const bool copy_requirements) :
    testing_area(testing_area),
    copy_requirements(copy_requirements),
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

    // Set up dropdown factories.
    current_test_group_dropdown->set_factory(configure_combo_box_factory(sigc::ptr_fun(&TestGroup::bind_name_to_label)));
    new_test_group_dropdown->set_factory(configure_combo_box_factory(sigc::ptr_fun(&TestGroup::bind_name_to_label)));
    requirement_dropdown->set_factory(configure_combo_box_factory(sigc::ptr_fun(&Requirement::bind_name_to_label)));
}

void TestingCopyToTestGroupPopover::popover_show() const
{
    set_new_test_group_dropdown();

    try {
        set_test_group_dropdown();
        set_requirement_dropdown();
    } catch (const std::runtime_error& exception) {
        current_test_group_dropdown->set_selected(0);
        requirement_dropdown->set_selected(0);

        popover_logger->warn("Could not determine selected Test Group and/or Requirement. Using sensible defaults.");
        popover_logger->warn(exception.what());
    }
}

void TestingCopyToTestGroupPopover::confirm_button_clicked() const noexcept
{
    my_popover->popdown();

    const auto source_requirement = std::dynamic_pointer_cast<Requirement>(requirement_dropdown->get_selected_item());
    const auto target_test_group = dynamic_cast<TestGroup *>(new_test_group_dropdown->get_selected_item().get());

    if (source_requirement == nullptr) {
        popover_logger->warn("Not copying Requirement, as no Requirement selected.");
        return;
    }

    if (target_test_group == nullptr) {
        popover_logger->warn("Not copying Requirement, as no destination Test Group selected.");
        return;
    }

    if (copy_requirements) {
        target_test_group->insert_object(source_requirement);
        popover_logger->debug("Copied Requirement \"" + source_requirement->property_name().get_value() +
            "\" into Test Group \"" + target_test_group->property_name().get_value() + "\".");
    } else {
        const auto source_test_group = dynamic_cast<TestGroup *>(current_test_group_dropdown->get_selected_item().get());
        if (source_test_group == nullptr) {
            popover_logger->warn("Not moving Requirement, as no source Test Group selected.");
            return;
        }

        source_test_group->delete_object(source_requirement);
        target_test_group->insert_object(source_requirement);
        popover_logger->debug("Moved Requirement \"" + source_requirement->property_name().get_value() +
            "\" from Test Group \"" +  + "\" into Test Group \"" + target_test_group->property_name().get_value() +
            "\".");
    }
}

void TestingCopyToTestGroupPopover::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from 'popover_show' callback.
void TestingCopyToTestGroupPopover::set_test_group_dropdown() const
{
    // Select the subsystem's test group model.
    const auto test_group_model = testing_area.get_active_subsystem()->get_test_groups();

    if (current_test_group_dropdown->get_model() != test_group_model)
        current_test_group_dropdown->set_model(test_group_model);

    // Get the relevant test group from the model by walking up to the parent-most node in the tree view.
    const auto selected_row = testing_area.get_selected_row();
    auto current_test_group_row = selected_row;

    while (current_test_group_row->get_depth() > 0)
        current_test_group_row = current_test_group_row->get_parent();

    const auto current_test_group = dynamic_cast<const TestGroup *>(current_test_group_row->get_item().get());
    if (current_test_group == nullptr)
        throw std::runtime_error("Something other than a TestGroup was present at the root level of the Test model.");

    /*
     * Locate the parent-most node in the test group model. A linear search over the relatively small list of test
     * groups is preferable to the complexity of an index-caching map as seen with ObjectGroupBase.
     */
    bool set = false;

    const auto test_group_count = test_group_model->get_n_items();
    for (guint test_group_index = 0; test_group_index < test_group_count; ++test_group_index) {
        const auto test_group = test_group_model->get_item(test_group_index);
        if (*test_group == *current_test_group) {
            set = true;
            current_test_group_dropdown->set_selected(test_group_index);
            break;
        }
    }

    if (set == false)
        throw std::runtime_error("The TestGroup \"" + current_test_group->property_name().get_value() +
            "\" was present in the tree but not the model.");
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from 'popover_show' callback.
void TestingCopyToTestGroupPopover::set_new_test_group_dropdown() const noexcept
{
    const auto test_group_model = testing_area.get_active_subsystem()->get_test_groups();
    if (new_test_group_dropdown->get_model() != test_group_model)
        new_test_group_dropdown->set_model(test_group_model);

    new_test_group_dropdown->set_selected(0);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from 'popover_show' callback.
void TestingCopyToTestGroupPopover::set_requirement_dropdown() const
{
    const auto selected_test_group = dynamic_cast<const TestGroup *>(
        current_test_group_dropdown->get_selected_item().get());

    if (selected_test_group == nullptr)
        throw std::runtime_error("Something other than a Test Group was present in the Test Group filter.");

    selected_test_group->populate_dropdown_model(*requirement_dropdown);
    requirement_dropdown->set_selected(0);
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
