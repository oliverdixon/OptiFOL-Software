/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the <i>Run Tests</i> popover in the <i>Testing and Compliance</i> area.
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#ifndef TESTINGRUNTESTSPOPOVER_HPP
#define TESTINGRUNTESTSPOPOVER_HPP

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/entry.h>
#include <gtkmm/notebook.h>
#include <gtkmm/popover.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <log4cxx/logger.h>

#include "../ProcessExecutor.hpp"

namespace optifol
{

class TestingArea;

/**
 * @class TestingRunTestsPopover
 * @brief Manage the <i>Run Tests</i> popover for the <i>Testing and Compliance</i> area.
 * @see TestingArea for the parent area
 *
 * @details
 *  The <i>Run Tests</i> popover provides controls for dry-running and executing unit test specifications through an
 *  external executable(s) for the selected TestGroup within the Subsystem. The following GTK elements are expected to
 *  be available from the given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>run_tests_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>run_tests_confirm</code></td>
 *          <td>Confirm execution of test executable</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>run_tests_cancel</code></td>
 *          <td>Cancels execution of test executables</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::DropDown</td>
 *          <td><code>test_backend_provider</code></td>
 *          <td>Selects the test backend provider, e.g. Google Test</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Entry</td>
 *          <td><code>test_group_name</code></td>
 *          <td>Read-only text area for the name of the selected TestGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Notebook</td>
 *          <td><code>run_tests_discovery_notebook</code></td>
 *          <td>Notebook placeholder for displaying dry-runs of discovered test executables within the TestGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Button</td>
 *          <td><code>run_tests_discover</code></td>
 *          <td>Discovers test executables required by the selected TestGroup executes dry-runs</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Box</td>
 *          <td><code>run_tests_discovery_box</code></td>
 *          <td>Area detaining <code>run_tests_discovery_notebook</code>, including any visual separators to be
 *              displayed only when the Gtk::Notebook has content.</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the expected
 *  type instantiations. */
class TestingRunTestsPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @param testing_area A mutating reference to the TestingArea of which the popover is a member
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    TestingRunTestsPopover(Gtk::Builder &builder, TestingArea &testing_area);

private:
    /**
     * @enum TestProvidersDropDown
     * @brief The mapping between <code>test_backend_provider</code> Gtk::DropDown model indices and the test provider.
     */
    enum class TestProvidersDropDown
    {
        GoogleTest = 0 /**< Google Test framework; use GoogleTestFactory. */
    };

    /**
     * @class DiscoveryPage
     * @brief A single page in a Gtk::Notebook holding the tab widget Gtk::Label and the scrollable Gtk::TextView.
     * @details The Gtk::Notebook widget does not own its child pages; to avoid memory access problems, always remove
     *  pages from the Gtk::Notebook before destructing the corresponding DiscoveryPage.
     */
    struct DiscoveryPage
    {
        explicit DiscoveryPage(const std::string &tab_name);

        Gtk::Label tab_label;
        Gtk::ScrolledWindow scrolled_window;
        Gtk::TextView text_view;
    };

    /**
     * @brief Handle a click of the <i>Confirm</i> by attempting to create a Requirement with the given characteristics.
     */
    void confirm_button_clicked() const noexcept;

    /**
     * @brief Handle a click of the <i>Cancel</i> button by discarding all input and closing the popover.
     */
    void cancel_button_clicked() const noexcept;

    /**
     * @brief Clear all user fields in the popover
     */
    void clear_inputs() const noexcept;

    /**
     * @brief Handle the popover becoming visible.
     */
    void show_popover() const noexcept;

    /**
     * @brief Handle a click of the <i>Discover Tests</i> button to dry-run the test executables and log the
     *  <code>stdout</code> and <code>stderr</code> in a Gtk::Notebook page.
     * @pre The number of stored pages in @ref discovery_pages is equal to the number of displayed pages in @ref
     *  discovery_notebook, prior to the mutation.
     * @post The number of stored pages in @ref discovery_pages is equal to the number of displayed pages in @ref
     *  discovery_notebook, following the mutation.
     */
    void discover_tests_clicked() noexcept;

    /**
     * @brief Execute the unit-testing framework for a dry run to discover available tests, check the version of the
     *  framework, etc, and stream output to a Gtk::TextBuffer nominally attached to a Gtk::TextView.
     * @param executable_name A view of the executable name; this should have a lifetime guarantee attached to the key
     *  of @ref discovery_executor_pool.
     * @param output_buffer The destination buffer to which the executable pipes should be streamed.
     */
    void discover_executable(std::string_view executable_name, Glib::RefPtr<Gtk::TextBuffer> output_buffer) noexcept;

    static const char *const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    TestingArea &testing_area;

    Gtk::Popover *const my_popover;
    Gtk::Button *const confirm_button;
    Gtk::Button *const cancel_button;
    Gtk::DropDown *const test_provider;
    Gtk::Entry *const test_group_name_entry;
    Gtk::Notebook *const discovery_notebook;
    Gtk::Button *const discover_button;
    Gtk::Box *const discovery_box;

    std::vector<DiscoveryPage> discovery_pages;
    std::unordered_map<std::string_view, std::unique_ptr<ProcessExecutor>> discovery_executor_pool;
};

} // namespace optifol

#endif // TESTINGRUNTESTSPOPOVER_HPP
