/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the ManageTestsPopover GUI popover
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef MANAGETESTSPOPOVER_HPP
#define MANAGETESTSPOPOVER_HPP

#include <giomm/liststore.h>
#include <gtkmm/button.h>
#include <gtkmm/columnview.h>
#include <gtkmm/entry.h>
#include <gtkmm/listitem.h>
#include <gtkmm/popover.h>
#include <gtkmm/singleselection.h>
#include <log4cxx/logger.h>
#include <unordered_set>

#include "../../Optifol.hpp"
#include "../../UserTesting/Discovery/DiscoveryTestExecutable.hpp"

namespace optifol
{

class Test;
class RequirementsIndexArea;
class TestSpecificationEntry;

/**
 * @class ManageTestsPopover
 * @brief Manage the test-management capability in the Requirement editor.
 * @see IndexNewRequirementPopover for a possible parent area
 * @see IndexEditRequirementPopover for a possible parent area
 *
 * @details
 *  The <i>Test Management</i> popover is a sub-popover of the IndexNewRequirementPopover or the
 *  IndexEditRequirementPopover. It provides controls to review and intelligently define software-level unit tests
 *  for a fixed Requirement i.a.w. a test executable. For the purposes of querying test executables, the popover also
 *  maintains a cache of Subsystem-agnostic discovery executables. The following GTK elements are expected from the
 *  given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>manage_tests_popover</code></td>
 *          <td>Managed popover</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>manage_tests_confirm</code></td>
 *          <td>Confirm test specifications</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>manage_tests_new_test</code></td>
 *          <td>Create a new TestSpecificationEntry</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>manage_tests_duplicate_test</code></td>
 *          <td>Duplicate the selected TestSpecificationEntry</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>manage_tests_delete_test</code></td>
 *          <td>Delete the selected TestSpecificationEntry</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnView</td>
 *          <td><code>manage_tests_view</code></td>
 *          <td>View for the dynamically populated TestSpecificationEntry list</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>manage_tests_target_exe</code></td>
 *          <td>View column for the target executable name Gtk::Entry</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>manage_tests_fixture</code></td>
 *          <td>View column for the target test fixture Gtk::DropDown</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>manage_tests_name</code></td>
 *          <td>View column for the target test case name Gtk::DropDown</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the expected
 *  type instantiations.
 */
class ManageTestsPopover : public sigc::trackable
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    explicit ManageTestsPopover(Gtk::Builder& builder);

    /**
     * @brief Sets a new model for the TestSpecificationEntry objects, discarding the existing one.
     * @param model The new model.
     */
    void set_model(const Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>>& model) noexcept;

    /**
     * @brief Provide a human-readable summary of a model containing tests or specifications thereof.
     * @param test_model The test model to query.
     * @see Test
     * @see TestSpecificationEntry
     */
    static std::string format_test_summary(const Gio::ListModel &test_model) noexcept;

private:
    static const char * const popover_name;
    static const log4cxx::LoggerPtr popover_logger;

    /**
     * @brief Handle a click of the <i>Confirm</i> button by hiding the popover. (The model is queried later.)
     */
    void confirm_button_clicked() const;

    void new_test_clicked() const;

    void delete_test_clicked() const;

    void duplicate_test_clicked() const;

    static void setup_fixtures_combo(const Glib::RefPtr<Gtk::ListItem> &list_item);

    void bind_test_executable(const Glib::RefPtr<Gtk::ListItem> &list_item);

    static void bind_test_fixture(const Glib::RefPtr<Gtk::ListItem>& list_item) noexcept;

    static void bind_test_name(const Glib::RefPtr<Gtk::ListItem>& list_item) noexcept;

    void handle_executable_change(const Glib::RefPtr<TestSpecificationEntry> &test_spec, const Gtk::Entry *exe_entry);

    Gtk::Popover * const popover;
    Gtk::Button * const confirm_button;
    Gtk::Button * const new_test_button;
    Gtk::Button * const duplicate_test_button;
    Gtk::Button * const delete_test_button;
    Gtk::ColumnView * const view;

    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gio::ListStore<TestSpecificationEntry>> test_spec_model;

    SharedUnorderedSet<DiscoveryTestExecutable> discovery_exe_cache;
};

} // namespace optifol

#endif // MANAGETESTSPOPOVER_HPP
