/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Main Window's Requirements Index UI area
 * @author Oliver Dixon
 * @date 2025-03-29
 * @version Development
 */

#ifndef REQUIREMENTSINDEXAREA_HPP
#define REQUIREMENTSINDEXAREA_HPP

#include <giomm/liststore.h>
#include <gtkmm/builder.h>
#include <gtkmm/columnview.h>
#include <gtkmm/listitem.h>
#include <gtkmm/singleselection.h>

#include "../../Storage/Requirement.hpp"
#include "../ContextButtonCorrespondence.hpp"
#include "../IWindowArea.hpp"
#include "IndexDeleteRequirementPopover.hpp"
#include "IndexDuplicateRequirementPopover.hpp"
#include "IndexEditRequirementPopover.hpp"
#include "IndexNewRequirementPopover.hpp"

namespace optifol
{

/**
 * @class RequirementsIndexArea
 * @brief Manage the <i>Requirements Index</i> area
 * @details
 *  <p>
 *      The <i>Requirements Index</i> area provides controls for reviewing and manipulating requirements for a single
 *      Subsystem. The basic set of operations on the atomic Requirement object includes creation, deletion,
 *      modification, and duplication. The following GTK elements are expected from the given Gtk::Builder:
 *      <table>
 *          <tr>
 *              <th>GTK C++ Class</th>
 *              <th>Unique Identifier</th>
 *              <th>Purpose</th>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Widget (abstract)</td>
 *              <td><code>requirements_index_advice_unselected</code></td>
 *              <td>Advice to display when the area is unavailable</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Widget (abstract)</td>
 *              <td><code>requirements_index_content</code></td>
 *              <td>Replacement to <code>requirements_index_advice_unselected</code>, containing all active content</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnView</td>
 *              <td><code>requirements_view</code></td>
 *              <td>Table containing Requirement entries and associated metadata</td>
 *          </tr>
 *          <tr>
 *              <td>Gio::Menu</td>
 *              <td><code>requirement_context_menu</code></td>
 *              <td>Area-wide context menu</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>new_requirement</code></td>
 *              <td>Button to active <i>New Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>new_requirement_popover</code></td>
 *              <td>The <i>New Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>edit_requirement</code></td>
 *              <td>Button to active <i>Edit Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>edit_requirement_popover</code></td>
 *              <td>The <i>Edit Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>duplicate_requirement</code></td>
 *              <td>Button to active <i>Duplicate Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>duplicate_requirement_popover</code></td>
 *              <td>The <i>Duplicate Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::MenuButton</td>
 *              <td><code>delete_requirement</code></td>
 *              <td>Button to active <i>Delete Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Popover</td>
 *              <td><code>delete_requirement_popover</code></td>
 *              <td>The <i>Delete Requirement</i> popover</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>requirement_name</code></td>
 *              <td>The Requirement name column</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>requirement_statement</code></td>
 *              <td>The Requirement FOL statement column</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>requirement_description</code></td>
 *              <td>The Requirement multi-line description column</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>requirement_priority</code></td>
 *              <td>The numerical Requirement priority column</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>requirement_created</code></td>
 *              <td>The Requirement created-on date/time column</td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::ColumnViewColumn</td>
 *              <td><code>requirement_modified</code></td>
 *              <td>The Requirement last-modified date/time column</td>
 *          </tr>
 *      </table>
 *  </p>
 *  <p>
 *      In addition to the stated required GTK elements, constituent popovers of this view will require their own,
 *      possibly distinct, set of elements:
 *      <ul>
 *          <li>@ref IndexNewRequirementPopover</li>
 *          <li>@ref IndexEditRequirementPopover</li>
 *          <li>@ref IndexDuplicateRequirementPopover</li>
 *          <li>@ref IndexDeleteRequirementPopover</li>
 *      </ul>
 *  </p>
 */
class RequirementsIndexArea :
        public IWindowArea
{
public:
    /**
     * @brief Construct a new compartmentalised area for displaying and managing sets of subsystem requirements
     * @param builder The GTK builder attached to the main window
     */
    explicit RequirementsIndexArea(Gtk::Builder& builder);

    void select_model(const Glib::RefPtr<const Subsystem> &subsystem_model) override;

    void deselect_model() override;

    const Subsystem *observe_active_subsystem() const noexcept override;

    guint get_selected_index() const override;

    /**
     * @brief Construct and add a Requirement in the internal data model for the associated Subsystem
     * @tparam CtorArgs Argument type vector to forward to the Requirement constructor
     * @param args Arguments to perfectly forward to the Requirement constructor
     * @note This member function is not vacuous; it removes the need to expose a non-constant reference to the
     *  SymbolRepository.
     */
    template<class... CtorArgs>
    void construct_and_add_requirement(CtorArgs&&... args)
    {
        data_model->append(Glib::make_refptr_for_instance(new Requirement(
            std::forward<CtorArgs>(args)...,
            symbol_repository
        )));
    }

    /**
     * @brief Bind a Requirement description attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_description(const Glib::RefPtr<Gtk::ListItem> &list_item);

    /**
     * @brief Bind a Requirement statement attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_statement(const Glib::RefPtr<Gtk::ListItem> &list_item);

    /**
     * @brief Bind a Requirement priority attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_priority(const Glib::RefPtr<Gtk::ListItem> &list_item);

    /**
     * @brief Bind a Requirement CNF-normalised attribute to a label
     * @param list_item The container in which the destination label exists
     */
    static void on_bind_property_normalised(const Glib::RefPtr<Gtk::ListItem> &list_item);

private:
    Glib::RefPtr<const Subsystem> active_subsystem;
    Glib::RefPtr<Gio::ListStore<Requirement>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();

    std::pair<Gtk::Widget*, Gtk::Widget*> on_off_widgets;

    static const char * const area_name;

    Gtk::ColumnView * view;

    ContextButtonCorrespondence context_menu;

    SymbolRepository symbol_repository;

    IndexNewRequirementPopover new_requirement_popover;
    IndexEditRequirementPopover edit_requirement_popover;
    IndexDuplicateRequirementPopover duplicate_requirement_popover;
    IndexDeleteRequirementPopover delete_requirement_popover;
};

}

#endif
