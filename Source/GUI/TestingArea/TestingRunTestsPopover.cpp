/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/26/25.
//

#include "TestingRunTestsPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "TestingArea.hpp"

namespace optifol
{

const char *const TestingRunTestsPopover::popover_name = "Run Tests Popover";
const log4cxx::LoggerPtr TestingRunTestsPopover::popover_logger =
        Logging::get_logger({"GUI", "TestingCompliance", "RunTests"});

TestingRunTestsPopover::TestingRunTestsPopover(Gtk::Builder &builder, TestingArea &testing_area) :
    testing_area(testing_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "run_tests_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "run_tests_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "run_tests_cancel")),
    test_group_name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "test_group_name")),
    discovery_notebook(GTKHelpers::get_widget<Gtk::Notebook>(popover_name, builder, "run_tests_discovery_notebook")),
    discover_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "run_tests_discover")),
    discovery_box(GTKHelpers::get_widget<Gtk::Box>(popover_name, builder, "run_tests_discovery_box"))
{
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::show_popover));
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::cancel_button_clicked));
    discover_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::discover_tests_clicked));

    discovery_notebook->signal_page_added().connect([this](Gtk::Widget * const, const guint)
    {
        discovery_box->set_visible();
    });

    discovery_notebook->signal_page_removed().connect([this](Gtk::Widget * const, const guint)
    {
        if (discovery_notebook->get_n_pages() == 0)
            discovery_box->set_visible(false);
    });
}

TestingRunTestsPopover::DiscoveryPage::DiscoveryPage(const std::string &tab_name)
{
    tab_label.set_text(tab_name);
    scrolled_window.set_child(text_view);
    text_view.set_editable(false);
    text_view.add_css_class("optifol_monospace");
}

void TestingRunTestsPopover::confirm_button_clicked() const
{

}

void TestingRunTestsPopover::cancel_button_clicked() const
{
    my_popover->popdown();
    clear_inputs();
}

void TestingRunTestsPopover::clear_inputs() const
{
    test_group_name_entry->set_text("");
}

void TestingRunTestsPopover::show_popover()
{
    const auto selected = testing_area.get_selection();
    test_group_name_entry->set_text(selected->property_name().get_value());
}

void TestingRunTestsPopover::discover_tests_clicked()
{
    const auto existing_page_count = static_cast<guint>(discovery_pages.size());

    // Pre-condition: verify that the stored pages are synchronised with the displayed pages prior to mutation.
    assert(existing_page_count == static_cast<guint>(discovery_notebook->get_n_pages()));

    const auto selected_test_group = testing_area.get_selection();
    const auto begin = selected_test_group->begin_executable_groups();
    const auto end = selected_test_group->end_executable_groups();

    guint page_number = 0;

    // Process each executable target required by the Test Group items, reusing notebook pages where possible.
    for (std::remove_const_t<decltype(begin)> exe_group_it = begin; exe_group_it != end; ++exe_group_it) {
        if (page_number < existing_page_count) {

            // Mutate the existing discovery label and window.
            auto& page = discovery_pages[page_number];
            page.tab_label.set_text(exe_group_it->first);
            discover_executable(exe_group_it->first, page.text_view.get_buffer());

        } else {

            // Create a new discovery label and window to be appended to the notebook.
            auto& page = discovery_pages.emplace_back(exe_group_it->first);
            discover_executable(exe_group_it->first, page.text_view.get_buffer());
            discovery_notebook->append_page(page.scrolled_window, page.tab_label);

        }

        ++page_number;
    }

    /*
     * Delete excess pages. Those are stored/displayed pages that cannot be repurposed and are not shown in the
     * notebook; i.e., the tail of the stored pages that have not been created or mutated by the previous operation. One
     * past the head of needed pages is (zero-indexed) counted by `page_number`; thus we need to keep the first
     * `page_number - 1` pages, and everything further should be discarded.
     */
    if (discovery_pages.empty() == false) {

        const auto last_stored_item_index = static_cast<guint>(discovery_pages.size()) - 1;

        for (guint index = last_stored_item_index; index > page_number; --index) {
            discovery_notebook->remove_page(static_cast<int>(index));
            discovery_pages.pop_back();
        }

        if (last_stored_item_index == page_number) {
            discovery_notebook->remove_page(static_cast<int>(page_number));
            discovery_pages.pop_back();
        }
    }

    // Post-condition: verify that the stored pages are synchronised with the displayed pages following mutation.
    assert(static_cast<guint>(discovery_pages.size()) == static_cast<guint>(discovery_notebook->get_n_pages()));
}

void TestingRunTestsPopover::discover_executable(
        const std::string &executable_name, const Glib::RefPtr<Gtk::TextBuffer> &output_buffer)
{
    auto executor = std::make_unique<ProcessExecutor>(
        "",
        std::vector<std::string>{std::string(executable_name), "--gtest_list_tests" },
        std::vector<std::string>{},
        output_buffer,
        [this, executable_name](const int)
        {
            discovery_executor_pool.erase(executable_name);
        }
    );

    discovery_executor_pool.emplace(executable_name, std::move(executor));
}

} // namespace optifol
