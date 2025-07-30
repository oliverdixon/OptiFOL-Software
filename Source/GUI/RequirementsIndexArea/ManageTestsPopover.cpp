/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/29/25.
//

#include "ManageTestsPopover.hpp"

#include <iostream>

#include "../../Storage/Test.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "GoogleTestExecutable.hpp"

namespace optifol
{

const char *const ManageTestsPopover::popover_name = "New Requirement Popover";
const log4cxx::LoggerPtr ManageTestsPopover::popover_logger =
        Logging::get_logger({"GUI", "RequirementsIndex", "NewRequirement", "TestManagement"});

ManageTestsPopover::ManageTestsPopover(RequirementsIndexArea &parent_area, Gtk::Builder &builder) :
    parent_area(parent_area),
    popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "manage_tests_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_confirm")),
    new_test_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_new_test")),
    duplicate_test_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_duplicate_test")),
    delete_test_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "manage_tests_delete_test")),
    view(GTKHelpers::get_widget<Gtk::ColumnView>(popover_name, builder, "manage_tests_view")),
    test_spec_model(Gio::ListStore<Test>::create())
{
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &ManageTestsPopover::confirm_button_clicked));
    new_test_button->signal_clicked().connect(sigc::mem_fun(*this, &ManageTestsPopover::new_test_clicked));

    selection_model->set_model(test_spec_model);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);

    view->set_model(selection_model);

    const auto columns = view->get_columns();
    const auto column_count = columns->get_n_items();
    guint processed_columns = 0;

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto& gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "manage_tests_target_exe") {
                factory->signal_setup().connect(sigc::ptr_fun(&GTKHelpers::setup_entry));
                factory->signal_bind().connect(sigc::ptr_fun(&ManageTestsPopover::bind_test_executable));
            } else if (gtk_id == "manage_tests_fixture") {
                factory->signal_setup().connect(sigc::ptr_fun(&GTKHelpers::setup_combo_box));
                factory->signal_bind().connect(sigc::ptr_fun(&ManageTestsPopover::bind_text_fixture));
            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
            ++processed_columns;
        }
    }
}

void ManageTestsPopover::confirm_button_clicked() const
{
    popover->popdown();
}

void ManageTestsPopover::new_test_clicked() const
{
    test_spec_model->append(Glib::make_refptr_for_instance(new Test()));
}

void ManageTestsPopover::delete_test_clicked() const
{
    test_spec_model->remove(selection_model->get_selected());
}

void ManageTestsPopover::bind_test_executable(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto exe_entry = dynamic_cast<Gtk::Entry *>(list_item->get_child());
    const auto test_item = std::dynamic_pointer_cast<Test>(list_item->get_item());

    if (exe_entry != nullptr && test_item != nullptr) {
        Glib::Binding::bind_property(
            test_item->property_target_executable_name(),
            exe_entry->property_text(),
            Glib::Binding::Flags::SYNC_CREATE,
            [test_item](const Glib::ustring& exe_name) -> std::optional<Glib::ustring>
            {
                std::ignore = exe_name;
                if (test_item == nullptr)
                    return "";

                return exe_name;
            }
        );

        exe_entry->signal_activate().connect([exe_entry, test_item]
        {
            test_item->share_test_executable(std::make_shared<GoogleTestExecutable>(exe_entry->get_text()));
        });
    }
}

void ManageTestsPopover::bind_text_fixture(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto fixture_combo = dynamic_cast<Gtk::DropDown *>(list_item->get_child());
    const auto test_item = std::dynamic_pointer_cast<Test>(list_item->get_item());

    if (fixture_combo != nullptr && test_item != nullptr)
        Glib::Binding::bind_property(
            test_item->property_target_executable_name(),
            fixture_combo->property_model(),
            Glib::Binding::Flags::SYNC_CREATE,
            [test_item](const Glib::ustring&exe_name) -> std::optional<Glib::RefPtr<Gtk::StringList>>
            {
                std::ignore = exe_name;
                const auto exe = test_item->observe_test_executable();
                if (exe == nullptr)
                    return {};

                return exe->get_fixtures_model();
            }
        );
}

} // namespace optifol
