/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the <i>Run Tests</i> popover in the <i>Testing and Compliance</i> area.
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#include "TestingRunTestsPopover.hpp"

#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../StreamingProcessExecutor.hpp"
#include "GoogleTestFactory.hpp"
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
    test_provider(GTKHelpers::get_widget<Gtk::DropDown>(popover_name, builder, "test_backend_provider")),
    test_group_name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "test_group_name")),
    discovery_notebook(GTKHelpers::get_widget<Gtk::Notebook>(popover_name, builder, "run_tests_discovery_notebook")),
    discover_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "run_tests_discover")),
    discovery_box(GTKHelpers::get_widget<Gtk::Box>(popover_name, builder, "run_tests_discovery_box"))
{
    my_popover->signal_show().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::show_popover));
    confirm_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::cancel_button_clicked));
    discover_button->signal_clicked().connect(sigc::mem_fun(*this, &TestingRunTestsPopover::discover_tests_clicked));

    discovery_notebook->signal_page_added().connect([this](Gtk::Widget * const, const guint) noexcept
    {
        discovery_box->set_visible();
    });

    discovery_notebook->signal_page_removed().connect([this](Gtk::Widget * const, const guint) noexcept
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

void TestingRunTestsPopover::confirm_button_clicked() noexcept
{
    // TODO URGENT: verify noexcept property of this and the lambda.
    const auto selected_test_group = testing_area.get_selection();
    const auto begin = selected_test_group->begin_executable_groups();
    const auto end = selected_test_group->end_executable_groups();

    for (std::remove_const_t<decltype(begin)> exe_group_it = begin; exe_group_it != end; ++exe_group_it) {
        auto [executor, listener] = GoogleTestFactory::execute_test_group(
            exe_group_it->first,
            "FOLParserTest.*", // TODO URGENT: take test specification from TestGroup.
            [this, executable_name = exe_group_it->first](const int pid)
            {
                if (test_runner_pool.erase(executable_name) != 1)
                    // We should always be removing exactly one executor instance, as this is the process-exit callback.
                    popover_logger->warn("Test runner pool failed integrity check: more than one entry for executable "
                        + executable_name + " running under PID " + std::to_string(pid) + '.');

                if (test_runner_pool.empty())
                    // Once all test executables have completed, spin over the Requirements and assign TestResults.
                    distribute_test_results();
            }
        );

        test_runner_pool.emplace(exe_group_it->first, std::move(executor));
        listener_pool.push_back(std::move(listener));
    }
}

void TestingRunTestsPopover::cancel_button_clicked() const noexcept
{
    my_popover->popdown();
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void TestingRunTestsPopover::clear_inputs() const noexcept
{
    test_group_name_entry->set_text("");
}

void TestingRunTestsPopover::show_popover() const noexcept
{
    try {
        test_group_name_entry->set_text(testing_area.get_selection()->property_name().get_value());
    } catch (const std::runtime_error& selection_error) {
        popover_logger->error("Could not discover the selected Test Group entry.");
        popover_logger->error(selection_error.what());
    }
}

void TestingRunTestsPopover::discover_tests_clicked() noexcept
{
    const auto existing_page_count = static_cast<guint>(discovery_pages.size());

    // Pre-condition: verify that the stored pages are synchronised with the displayed pages prior to mutation.
    assert(existing_page_count == static_cast<guint>(discovery_notebook->get_n_pages()));

    guint page_number = 0;

    try {
        const auto selected_test_group = testing_area.get_selection();
        const auto begin = selected_test_group->begin_executable_groups();
        const auto end = selected_test_group->end_executable_groups();

        // Process each executable target required by the Test Group items, reusing notebook pages where possible.
        for (std::remove_const_t<decltype(begin)> exe_group_it = begin; exe_group_it != end; ++exe_group_it) {
            if (page_number < existing_page_count) {

                // Mutate the existing discovery label and window.
                auto& page = discovery_pages[page_number];
                page.tab_label.set_text(exe_group_it->first);
                page.text_view.get_buffer()->set_text("");
                discover_executable(exe_group_it->first, page.text_view.get_buffer());

            } else {

                // Create a new discovery label and window to be appended to the notebook.
                auto& page = discovery_pages.emplace_back(exe_group_it->first);
                discover_executable(exe_group_it->first, page.text_view.get_buffer());
                discovery_notebook->append_page(page.scrolled_window, page.tab_label);

            }

            ++page_number;
        }
    } catch (const std::runtime_error& selection_error) {
        popover_logger->error("Could not discover the selected Test Group entry.");
        popover_logger->error(selection_error.what());
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

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void TestingRunTestsPopover::discover_executable(
        const std::string_view executable_name, const Glib::RefPtr<Gtk::TextBuffer> &output_buffer) noexcept
{
    switch (test_provider->get_selected()) {
    case std::to_underlying(TestProvidersDropDown::GoogleTest):
        // Exception properties of std::unordered_map::emplace are unclear. Also test factories are not noexcept.
        try {
            discover_button->set_sensitive(false);
            discovery_executor_pool.emplace(executable_name,
                GoogleTestFactory::dry_run_executable(executable_name, output_buffer,
                    [this, executable_name](const int) noexcept
                    {
                        /*
                         * This is invoked from a Glib callback, so must be noexcept.
                         *
                         * std::hash<std::string_view>::operator() and std::string_view::operator== can be assumed to be
                         * noexcept, thus std::unordered_map<std::string_view, ...> is noexcept.
                         */
                        discovery_executor_pool.erase(executable_name);
                        discover_button->set_sensitive(); // Make sure this is restored!
                    }
                )
            );
        } catch (const Glib::SpawnError &spawn_error) {
            discover_button->set_sensitive();
            StreamingProcessExecutor::write_exception_error(spawn_error, output_buffer);
            popover_logger->error("Could not dry-run test executable \"" + std::string(executable_name) + "\".");
            popover_logger->error(spawn_error.what());
        } catch (...) {
            discover_button->set_sensitive();
            popover_logger->warn(
                    "Could not record entry of test discovery executor for \"" + std::string(executable_name) + "\".");
        }

        break;

    default:
        popover_logger->error("Cannot execute dry-run for executable \"" + std::string(executable_name) +
                "\": unsupported test provider.");
    }
}

void TestingRunTestsPopover::distribute_test_results() const noexcept
{
    testing_area.get_selection()->for_each(
        [this](Requirement& requirement)
        {
            try {
                for (const auto& result_group : listener_pool)
                    result_group->endow_requirement(requirement);
            } catch (const SemanticException& semantic_exception) {
                popover_logger->warn("Could not assign TestResult to Requirement in active group-under-test.");
                popover_logger->warn(semantic_exception.what());
            }
        }
    );
}

} // namespace optifol
