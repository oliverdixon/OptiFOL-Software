/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Testing and Compliance Failed Test Results view
 * @author Oliver Dixon
 * @date 2025-08-10
 * @version Development
 */

#ifndef OPTIFOL_TESTINGFAILEDVIEW_HPP
#define OPTIFOL_TESTINGFAILEDVIEW_HPP

#include <glibmm/refptr.h>
#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>
#include <gtkmm/noselection.h>
#include <gtkmm/treelistmodel.h>
#include <log4cxx/logger.h>

#include "../IWindowArea.hpp"

namespace optifol
{

/**
 * @class TestingFailedView
 * @brief Manage the <i>Failed Tests View</i> section of the <i>Testing and Compliance</i> area.
 *
 * @details
 *  The <i>Failed Tests View</i> presents a read-only tabular representation of PartialTestResult objects,
 * grouped by their respective owning Test objects, which are in turn grouped by the TestGroup objects. The
 * following GTK elements are expected from the given Gtk::Builder: <table> <tr> <th>GTK C++ Class</th>
 *          <th>Unique Identifier</th>
 *          <th>Purpose</th>
 *      </tr>
 *      <tr>
 *          <td>Gtk::Widget (abstract)</td>
 *          <td><code>test_groups_failed_container</code></td>
 *          <td>Containing widget of the entire sub-area.</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnView</td>
 *          <td><code>test_groups_failed_view</code></td>
 *          <td>Table detained by <code>test_groups_failed_container</code> tabulating PartialTestResult
 * fields.</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>failed_test_message</code></td>
 *          <td>Table column to display the message of the PartialTestResult, or the name of the Test
 * thereof.</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>failed_test_source_file</code></td>
 *          <td>Table column to display the source file name of the PartialTestResult.</td>
 *      </tr>
 *      <tr>
 *          <td>Gtk::ColumnViewColumn</td>
 *          <td><code>failed_test_source_line</code></td>
 *          <td>Table column to display the source line number of the PartialTestResult.</td>
 *      </tr>
 *  </table>
 *  A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in the
 * expected type instantiations.
 */
class TestingFailedView : public IWindowArea
{
public:
    /**
     * @brief Construct a new sub-area manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    explicit TestingFailedView(Gtk::Builder &builder);

    Subsystem *get_active_subsystem() noexcept override;

    const Subsystem *observe_active_subsystem() const noexcept override;

    void select_model(const Glib::RefPtr<Subsystem> &new_subsystem) override;

    void deselect_model() override;

private:
    void configure_columns() const;

    /**
     * @brief Handles a change in the model. In particular, show or hide the entire @ref container viewing
     * pane.
     * @param initial_index The initial index of the model changes.
     * @param removed_count The number of removed items.
     * @param added_count The number of added items.
     */
    void handle_model_change(guint initial_index, guint removed_count, guint added_count) const noexcept;

    static const log4cxx::LoggerPtr area_logger;
    static const char *const area_name;

    Gtk::Widget *const container;
    Gtk::ColumnView *const table;

    Glib::RefPtr<Subsystem> active_subsystem;
    Glib::RefPtr<Gtk::NoSelection> selection_model = Gtk::NoSelection::create();
    Glib::RefPtr<Gtk::TreeListModel> tree_model;
};

} // namespace optifol

#endif // OPTIFOL_TESTINGFAILEDVIEW_HPP
