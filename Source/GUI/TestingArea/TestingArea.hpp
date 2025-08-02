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
#include "TestingRunTestsPopover.hpp"

namespace optifol
{

/**
 * @class TestingArea
 * @brief Manage the <i>Testing and Compliance</i> area
 *
 * @details
 *  <p>
 *      The <i>Testing and Compliance</i> area provides controls for aggregating existing Requirement objects into
 *      TestGroup objects, and then executing unit test frameworks over the groups. The results of Requirement-wise unit
 *      tests can be reviewed in the area, or exported to a report using the <i>Releases and Reports</i> area
 *      capabilities. The following GTK elements are expected from the given Gtk::Builder:
 *      <table>
 *          <tr>
 *              <th>GTK C++ Class</th>
 *              <th>Unique Identifier</th>
 *              <th>Purpose</th>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Widget (abstract)</td>
 *              <td><code>testing_advice_unselected</code></td>
 *              <td>Advice to display when the area is unavailable</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnView</td>
 *              <td><code>test_groups_view</code></td>
 *              <td>Table to display nested TestGroup content</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>run_tests</code></td>
 *              <td>Button for executing the external testing framework on the selected TestGroup</td>
 *          </tr>
 *          <tr>
 *              <td>GtkColumnViewColumn</td>
 *              <td><code>test_requirement_name</code></td>
 *              <td>Table column to display the name of the Requirement associated with the Test</td>
 *          </tr>
 *          <tr>
 *              <td>GtkColumnViewColumn</td>
 *              <td><code>test_target_executable</code></td>
 *              <td>Table column to display the target executable of the Test</td>
 *          </tr>
 *          <tr>
 *              <td>GtkColumnViewColumn</td>
 *              <td><code>test_fixture</code></td>
 *              <td>Table column to display the test fixture of the Test</td>
 *          </tr>
 *          <tr>
 *              <td>GtkColumnViewColumn</td>
 *              <td><code>test_status</code></td>
 *              <td>Table column to display the iconised result of the lastest Test run</td>
 *          </tr>
 *      </table>
 *  </p>
 */
class TestingArea : public IWindowArea
{
public:
    explicit TestingArea(Gtk::Builder &builder);

    void select_model(const Glib::RefPtr<Subsystem> &new_subsystem) override;

    void deselect_model() override;

    Subsystem *observe_active_subsystem() noexcept override;

    const Subsystem *observe_active_subsystem() const noexcept override;

    /**
     * @brief Gets the selected TestGroup object in the model view.
     * @return The single selected TestGroup object in the model view.
     * @throws std::runtime_error There was no suitable TestGroup object selected.
     */
    Glib::RefPtr<TestGroup> get_selection() const;

private:
    template<typename ReturnType>
    using TestGetter = Glib::PropertyProxy_ReadOnly<ReturnType> (Test::*)() const;

    void configure_columns() const;

    void configure_selection_model() const;

    static const log4cxx::LoggerPtr area_logger;
    static const char *const area_name;

    Gtk::ColumnView *const test_groups_view;
    ContextButtonCorrespondence context_menu;
    std::pair<Gtk::Widget *, Gtk::Widget *> on_off_widgets;

    Glib::RefPtr<Subsystem> active_subsystem;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gtk::TreeListModel> tree_model;

    TestingRunTestsPopover run_tests_popover;
};

} // namespace optifol

#endif // TESTINGAREA_HPP
