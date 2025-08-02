/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/29/25.
//

#include <iostream>

#include "../../UserTesting/Discovery/GoogleTestDiscoveryExecutable.hpp"
#include "../../UserTesting/GUIModelling/Test.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "ManageTestsPopover.hpp"
#include "RequirementsIndexArea.hpp"

namespace optifol
{

const char *const ManageTestsPopover::popover_name = "New Requirement Popover";
const log4cxx::LoggerPtr ManageTestsPopover::popover_logger =
        Logging::get_logger({"GUI", "RequirementsIndex", "NewRequirement", "TestManagement"});

ManageTestsPopover::ManageTestsPopover(Gtk::Builder &builder) :
    popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "manage_tests_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_confirm")),
    new_test_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_new_test")),
    duplicate_test_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_duplicate_test")),
    delete_test_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_delete_test")),
    view(GTKHelpers::get_widget<Gtk::ColumnView>(popover_name, builder, "manage_tests_view"))
{
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &ManageTestsPopover::confirm_button_clicked));
    new_test_button->signal_clicked().connect(sigc::mem_fun(*this, &ManageTestsPopover::new_test_clicked));

    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);

    view->set_model(selection_model);

    const auto columns = view->get_columns();
    const auto column_count = columns->get_n_items();
    guint processed_columns = 0;

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto &gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "manage_tests_target_exe") {
                factory->signal_setup().connect(sigc::ptr_fun(&GTKHelpers::setup_entry));
                factory->signal_bind().connect(sigc::mem_fun(*this, &ManageTestsPopover::bind_test_executable));
            } else if (gtk_id == "manage_tests_fixture") {
                factory->signal_setup().connect(sigc::ptr_fun(&ManageTestsPopover::setup_fixtures_combo));
                factory->signal_bind().connect(sigc::ptr_fun(&ManageTestsPopover::bind_test_fixture));
            } else if (gtk_id == "manage_tests_name") {
                factory->signal_setup().connect(sigc::ptr_fun(&GTKHelpers::setup_combo_box));
                factory->signal_bind().connect(sigc::ptr_fun(&ManageTestsPopover::bind_test_name));
            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
            ++processed_columns;
        }
    }
}

void ManageTestsPopover::set_model(const Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> &model)
{
    test_spec_model = model;
    selection_model->set_model(test_spec_model);
}

void ManageTestsPopover::confirm_button_clicked() const
{
    popover->popdown();
}

void ManageTestsPopover::new_test_clicked() const
{
    test_spec_model->append(Glib::make_refptr_for_instance<TestSpecificationEntry>(new TestSpecificationEntry()));
}

void ManageTestsPopover::delete_test_clicked() const
{
    test_spec_model->remove(selection_model->get_selected());
}

void ManageTestsPopover::setup_fixtures_combo(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto combo_box = Gtk::make_managed<Gtk::DropDown>();
    combo_box->set_halign(Gtk::Align::START);
    list_item->set_child(*combo_box);

    const auto factory = Gtk::SignalListItemFactory::create();

    factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, false));
    factory->signal_bind().connect(
        [](const Glib::RefPtr<Gtk::ListItem>& selected_item)
        {
            const auto target_label = dynamic_cast<Gtk::Label *>(selected_item->get_child());
            const auto typed_fixture = std::dynamic_pointer_cast<DiscoveryTestFixture>(selected_item->get_item());
            if (target_label == nullptr || typed_fixture == nullptr)
                return;

            target_label->set_text(typed_fixture->property_name().get_value());
        }
    );

    combo_box->set_factory(factory);
}

void ManageTestsPopover::bind_test_executable(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto exe_entry = dynamic_cast<Gtk::Entry *>(list_item->get_child());
    const auto test_spec = std::dynamic_pointer_cast<TestSpecificationEntry>(list_item->get_item());

    if (exe_entry == nullptr || test_spec == nullptr)
        return;

    // Set-up the binding from the TestSpecificationEntry object to the GUI, such that model changes are visible.
    Glib::Binding::bind_property(
        test_spec->property_executable(),
        exe_entry->property_text(),
        Glib::Binding::Flags::SYNC_CREATE,
        [](const Glib::RefPtr<DiscoveryTestExecutable>& exe) -> std::optional<Glib::ustring>
        {
            if (exe == nullptr)
                return "";

            return exe->property_name().get_value();
        }
    );

    // Set-up the binding from the view to the model, such that committed changes are propagated and handled.
    exe_entry->signal_activate().connect(sigc::bind(
        sigc::mem_fun(*this, &ManageTestsPopover::handle_executable_change),
        test_spec,
        exe_entry
    ));
}

void ManageTestsPopover::bind_test_fixture(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto fixture_combo = dynamic_cast<Gtk::DropDown *>(list_item->get_child());
    const auto test_spec = std::dynamic_pointer_cast<TestSpecificationEntry>(list_item->get_item());

    if (fixture_combo == nullptr || test_spec == nullptr)
        return;

    /*
     * Set-up the binding from the TestSpecificationEntry object to the GUI, such that changes to the executable will
     * propagate to the fixtures combo box by loading the correct fixture model. This needn't be a bidirectional binding
     * as the combo box model would only ever change due to a change in the TestSpecificationEntry.
     */
    Glib::Binding::bind_property(
        test_spec->property_executable(),
        fixture_combo->property_model(),
        Glib::Binding::Flags::SYNC_CREATE,
        [](const Glib::RefPtr<DiscoveryTestExecutable> &exe) -> std::optional<Glib::RefPtr<Gio::ListModel>>
        {
            if (exe == nullptr)
                return {};

            return exe->get_fixture_model();
        }
    );

    // Set-up the binding from the model to the view, such that committed changes propagate to the fixture combo box.
    Glib::Binding::bind_property(
        fixture_combo->property_selected_item(),
        test_spec->property_fixture(),
        Glib::Binding::Flags::SYNC_CREATE,
        [](const Glib::RefPtr<Glib::ObjectBase> &selected_item)
                -> std::optional<Glib::RefPtr<DiscoveryTestFixture>>
        {
            const auto &typed_entry = std::dynamic_pointer_cast<DiscoveryTestFixture>(selected_item);
            if (typed_entry == nullptr)
                return {};

            return typed_entry;
        }
    );
}

void ManageTestsPopover::bind_test_name(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto test_combo = dynamic_cast<Gtk::DropDown *>(list_item->get_child());
    const auto test_spec = std::dynamic_pointer_cast<TestSpecificationEntry>(list_item->get_item());

    Glib::Binding::bind_property(
        test_spec->property_fixture(),
        test_combo->property_model(),
        Glib::Binding::Flags::SYNC_CREATE,
        [](const Glib::RefPtr<DiscoveryTestFixture> &fixture) -> std::optional<Glib::RefPtr<Gtk::StringList>>
        {
            if (fixture == nullptr)
                return {};

            return fixture->get_test_model();
        }
    );

    Glib::Binding::bind_property(
        test_combo->property_selected_item(),
        test_spec->property_name(),
        Glib::Binding::Flags::SYNC_CREATE,
        [](const Glib::RefPtr<Glib::ObjectBase> &selected_item) -> std::optional<Glib::ustring>
        {
            const auto &typed_entry = dynamic_cast<Gtk::StringObject *>(selected_item.get());
            if (typed_entry == nullptr)
                return {};

            return typed_entry->get_string();
        }
    );
}

void ManageTestsPopover::handle_executable_change(const Glib::RefPtr<TestSpecificationEntry> &test_spec,
    const Gtk::Entry *const exe_entry)
{
    const auto cached_exe_it = discovery_exe_cache.find(exe_entry->get_text());

    if (cached_exe_it == discovery_exe_cache.cend()) {
        const auto& name = exe_entry->get_text();
        const auto [entry, was_added] = discovery_exe_cache.emplace(Glib::make_refptr_for_instance(
            new GoogleTestDiscoveryExecutable(name)));

        if (was_added) {
            popover_logger->debug("Created and cached new test executable for discovery: \"" + name + "\".");
            test_spec->property_executable().set_value(*entry);
        } else
            popover_logger->error("Could not cache new test executable for discovery: \"" + name + "\".");
    }
    else {
        test_spec->property_executable().set_value(*cached_exe_it);
        popover_logger->debug("Recovered test executable for discovery from the cache: \"" +
            (*cached_exe_it)->property_name().get_value() + "\".");
    }
}

} // namespace optifol
