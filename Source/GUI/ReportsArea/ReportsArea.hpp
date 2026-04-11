/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Main Window's Releases and Reports area
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#ifndef REPORTSAREA_HPP
#define REPORTSAREA_HPP

#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>

#include "../../Storage/Subsystem.hpp"
#include "../ContextButtonCorrespondence.hpp"
#include "../IWindowArea.hpp"
#include "ReportsAreaGenerateLaTeXPopover.hpp"

namespace optifol
{

/**
 * @class ReportsArea
 * @brief Manage the <i>Releases and Reports</i> area
 *
 * @details
 *  <p>
 *      The <i>Releases and Reports</i> area provides controls for reviewing baselined releases of the project
 * with a frozen set of Requirement objects, and also enables the automatic generation of reports to document
 * baselines. The following GTK elements are expected from the given Gtk::Builder: <table> <tr> <th>GTK C++
 * Class</th> <th>Unique Identifier</th> <th>Purpose</th>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Widget (abstract)</td>
 *              <td><code>reports_advice_unselected</code></td>
 *              <td>Advice to display when the area is unavailable</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Widget (abstract)</td>
 *              <td><code>reports_content</code></td>
 *              <td>Replacement to <code>reports_advice_unselected</code>, containing all active content</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnView</td>
 *              <td><code>reports_elements_view</code></td>
 *              <td>Table containing baselines and previously generated reports</td>
 *          </tr>
 *          <tr>
 *              <td>Gio::Menu</td>
 *              <td><code>reports_context_menu</code></td>
 *              <td>Area-wide context menu</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>reports_generate_latex</code></td>
 *              <td>Button for the LaTeX generation popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>reports_generate_latex_popover</code></td>
 *              <td>Popover for the LaTeX generation facility</td>
 *          </tr>
 *      </table>
 *      A @ref std::runtime_error will be thrown by the class constructor if any of these are inaccessible in
 * the expected type instantiations.
 *  </p>
 *  <p>
 *      In addition to the stated required GTK elements, constituent popovers of this view will require their
 * own, possibly distinct, set of elements: <ul> <li>@ref ReportsAreaGenerateLaTeXPopover</li>
 *      </ul>
 *  </p>
 */
class ReportsArea : public IWindowArea
{
public:
    explicit ReportsArea(Gtk::Builder &builder);

    void select_model(const Glib::RefPtr<Subsystem> &subsystem) override;

    void deselect_model() override;

    Subsystem *get_active_subsystem() noexcept override;

    const Subsystem *observe_active_subsystem() const noexcept override;

private:
    static const char *const area_name;

    std::pair<Gtk::Widget *, Gtk::Widget *> on_off_widgets;

    Gtk::ColumnView *view;

    ContextButtonCorrespondence context_menu;

    Glib::RefPtr<Subsystem> active_subsystem;

    ReportsAreaGenerateLaTeXPopover generate_latex_popover;
};

} // namespace optifol

#endif // REPORTSAREA_HPP
