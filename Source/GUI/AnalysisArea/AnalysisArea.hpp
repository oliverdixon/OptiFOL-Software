/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Analysis and Optimisation view in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-04-27
 * @version Development
 */

#ifndef ANALYSISAREA_HPP
#define ANALYSISAREA_HPP

#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/treelistmodel.h>

#include "../../Storage/AnalysisGroup.hpp"
#include "../ContextButtonCorrespondence.hpp"
#include "../IWindowArea.hpp"
#include "AnalysisAreaNewAnalysisGroupPopover.hpp"

namespace optifol
{

/**
 * @class AnalysisArea
 * @brief Manage the <i>Analysis and Optimisation</i> area
 * @details
 *  <p>
 *      The <i>Analysis and Optimisation</i> area provides controls for categorising Requirement objects from the
 *      RequirementsIndexArea into AnalysisGroup groups. Groups may then be individually subject to mathematical
 *      analysis under the FOL engine, with the results of analysis displayed in a tabbed paned view. The following GTK
 *      elements are expected from the given Gtk::Builder:
 *      <table>
 *          <tr>
 *              <th>GTK C++ Class</th>
 *              <th>Unique Identifier</th>
 *              <th>Purpose</th>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Widget (abstract)</td>
 *              <td><code>analysis_advice_unselected</code></td>
 *              <td>Advice to display when the area is unavailable</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Widget (abstract)</td>
 *              <td><code>analysis_index_content</code></td>
 *              <td>Replacement to <code>analysis_advice_unselected</code>, containing all active content</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnView</td>
 *              <td><code>analysis_groups_view</code></td>
 *              <td>Table containing AnalysisGroup and child Requirement entries</td>
 *          </tr>
 *          <tr>
 *              <td>Gio::Menu</td>
 *              <td><code>analysis_groups_context_menu</code></td>
 *              <td>Area-wide context menu</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>new_analysis_group</code></td>
 *              <td>Button for creating a New AnalysisGroup</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>new_analysis_group_popover</code></td>
 *              <td>Popover UI for creating a New AnalysisGroup</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>edit_analysis_group</code></td>
 *              <td>Button for creating a editing an existing AnalysisGroup</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>edit_analysis_group_popover</code></td>
 *              <td>Popover UI for editing an existing AnalysisGroup</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>delete_analysis_group</code></td>
 *              <td>Button for creating a deleting an existing AnalysisGroup</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>delete_analysis_group_popover</code></td>
 *              <td>Popover UI for deleting an existing AnalysisGroup</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>analysis_requirement_name</code></td>
 *              <td>Table column for the Requirement / AnalysisGroup name</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>analysis_requirement_statement</code></td>
 *              <td>Table column for the Requirement statement</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>analysis_requirement_cnf</code></td>
 *              <td>Table column for the Requirement statement in normalised CNF</td>
 *          </tr>
 *      </table>
 *      A @ref std::runtime_error will be thrown by AnalysisArea(Gtk::Builder&) if any of these are inaccessible in the
 *      expected type instantiations.
 *  </p>
 *  <p>
 *      In addition to the stated required GTK elements, constituent popovers of this view will require their own,
 *      possibly distinct, set of elements:
 *      <ul>
 *          <li>@ref AnalysisAreaNewAnalysisGroupPopover</li>
 *      </ul>
 *  </p>
 */
class AnalysisArea :
        public IWindowArea
{
public:
    /**
      * @brief Construct a new compartmentalised area for displaying and managing sets of subsystem requirements
      * @param builder The GTK builder attached to the main window
      */
    explicit AnalysisArea(Gtk::Builder& builder);

    void select_model(const Glib::RefPtr<const Subsystem> &subsystem_model) override;

    void deselect_model() override;

    const Subsystem *observe_active_subsystem() const noexcept override;

private:
    static Glib::RefPtr<Gio::ListModel> analysis_group_expand(const Glib::RefPtr<Glib::ObjectBase> &item);

    static const char * const area_name;

    Glib::RefPtr<const Subsystem> active_subsystem;
    Glib::RefPtr<Gio::ListStore<AnalysisGroup>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gtk::TreeListModel> tree_model;

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;

    Gtk::ColumnView * groups_view;

    ContextButtonCorrespondence context_menu;

    AnalysisAreaNewAnalysisGroupPopover new_analysis_group_popover;
};

}

#endif
