/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Main Window's Testing UI area
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#ifndef TESTINGAREA_HPP
#define TESTINGAREA_HPP

#include <glibmm/refptr.h>
#include <gtkmm/columnview.h>
#include <gtkmm/label.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/treelistmodel.h>

#include "../../Storage/Subsystem.hpp"
#include "../../UserTesting/Execution/PayloadManagement/GoogleTestListener.hpp"
#include "../ContextButtonCorrespondence.hpp"
#include "../IWindowArea.hpp"
#include "TestingCopyToTestGroupPopover.hpp"
#include "TestingDeleteTestGroupPopover.hpp"
#include "TestingMoveToTestGroupPopover.hpp"
#include "TestingNewTestGroupPopover.hpp"
#include "TestingRenameTestGroupPopover.hpp"
#include "TestingRunTestsPopover.hpp"

namespace optifol
{

/**
 * @class TestingArea
 * @brief Manage the <i>Testing and Compliance</i> area
 *
 * @details
 *  The <i>Testing and Compliance</i> area provides controls for aggregating existing Requirement objects into
 *  TestGroup objects, and then executing unit test frameworks over the groups. The results of Requirement-wise unit
 *  tests can be reviewed in the area, or exported to a report using the <i>Releases and Reports</i> area
 *  capabilities. The following GTK elements are expected from the given Gtk::Builder:
 *  <table>
 *      <tr>
 *          <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Widget (abstract)</td>
 *          <td><code>testing_advice_unselected</code></td>
 *          <td>Advice to display when the area is unavailable</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Widget (abstract)</td>
 *          <td><code>testing_content</code></td>
 *          <td>Replacement to <code>testing_advice_unselected</code>, containing all active content</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnView</td>
 *          <td><code>test_groups_view</code></td>
 *          <td>Table to display nested TestGroup content</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>run_tests_popover</code></td>
 *          <td>Popover for executing the Test items in the selected TestGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>run_tests</code></td>
 *          <td>Button for executing the external testing framework on the selected TestGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>test_requirement_name</code></td>
 *          <td>Table column to display the name of the Requirement associated with the Test</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>test_target_executable</code></td>
 *          <td>Table column to display the target executable of the Test</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>test_fixture</code></td>
 *          <td>Table column to display the test fixture of the Test</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>test_status</code></td>
 *          <td>Table column to display the iconised result of the lastest Test run</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>new_test_group_popover</code></td>
 *          <td>Popover for creating a new TestGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>new_test_group</code></td>
 *          <td>Button for opening <code>new_test_group_popover</code></td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>copy_to_test_group_popover</code></td>
 *          <td>Popover for copying a Requirement to a new TestGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>copy_to_test_group</code></td>
 *          <td>Button for opening <code>copy_to_test_group_popover</code></td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>move_to_test_group_popover</code></td>
 *          <td>Popover for moving a Requirement between TestGroup objects</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>move_to_test_group</code></td>
 *          <td>Button for opening <code>move_to_test_group_popover</code></td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Popover</td>
 *          <td><code>delete_test_group_popover</code></td>
 *          <td>Popover for deleting a TestGroup</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::MenuButton</td>
 *          <td><code>delete_test_group</code></td>
 *          <td>Button for opening <code>delete_test_group_popover</code></td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the expected
 *  type instantiations.
 */
class TestingArea : public IWindowArea
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    explicit TestingArea(Gtk::Builder &builder);

    void select_model(const Glib::RefPtr<Subsystem> &new_subsystem) override;

    void deselect_model() override;

    Subsystem *get_active_subsystem() noexcept override;

    const Subsystem *observe_active_subsystem() const noexcept override;

    Glib::RefPtr<Gtk::TreeListRow> get_selected_row() noexcept;

    Glib::RefPtr<const Gtk::TreeListRow> get_selected_row() const noexcept;

    /**
     * @brief Copies the ref-counted pointer to the selected TestGroup. If a Requirement or Test is selected, the owning
     *  TestGroup is returned.
     * @return The owning TestGroup of the selected object.
     * @throws std::runtime_error There is no selected object.
     */
    Glib::RefPtr<TestGroup> get_selected_test_group();

    /**
     * @brief Copies the ref-counted pointer to the selected Requirement. If a Test is selected, the owning Requirement
     *  is returned.
     * @return The owning Requirement of the selected Test.
     * @throws std::runtime_error There is no selected object, or the selected object is unsuitable.
     */
    Glib::RefPtr<Requirement> get_selected_requirement();

private:
    /**
     * @brief Configure Gtk::ColumnViewColumn objects in the @ref test_groups_view.
     */
    void configure_columns() const;

    /**
     * @brief Configure the @ref selection_model.
     */
    void configure_selection_model() const;

    static const log4cxx::LoggerPtr area_logger;
    static const char *const area_name;

    Gtk::ColumnView *const test_groups_view;
    ContextButtonCorrespondence context_menu;
    std::pair<Gtk::Widget *, Gtk::Widget *> on_off_widgets;

    Glib::RefPtr<Subsystem> active_subsystem;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gtk::TreeListModel> tree_model;

    TestingNewTestGroupPopover new_test_group_popover;
    TestingRenameTestGroupPopover rename_test_group_popover;
    TestingDeleteTestGroupPopover delete_test_group_popover;
    TestingCopyToTestGroupPopover copy_requirement_popover;
    TestingMoveToTestGroupPopover move_requirement_popover;
    TestingRunTestsPopover run_tests_popover;
};

} // namespace optifol

#endif // TESTINGAREA_HPP
