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

#include <glibmm/binding.h>
#include <glibmm/refptr.h>
#include <gtkmm/button.h>
#include <gtkmm/columnview.h>
#include <gtkmm/label.h>
#include <gtkmm/listitem.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/treelistmodel.h>

#include "../../DereferencingEqualityFunctor.hpp"
#include "../../Storage/Subsystem.hpp"
#include "../../UserTesting/Google/GoogleTestListener.hpp"
#include "../GTKHelpers.hpp"
#include "../IWindowArea.hpp"
#include "../ProcessExecutor.hpp"

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
 *              <td><code>test_suite</code></td>
 *              <td>Table column to display the constituent test suite of the Test</td>
 *          </tr>
 *          <tr>
 *              <td>GtkColumnViewColumn</td>
 *              <td><code>test_name</code></td>
 *              <td>Table column to display the name of the Test</td>
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

    void select_model(const Glib::RefPtr<const Subsystem> &new_subsystem) override;

    void deselect_model() override;

    const Subsystem *observe_active_subsystem() const noexcept override;

    guint get_selected_index() const override;

    void accept_new_result(std::unique_ptr<TestResult> &&test_result);

    void propagate_pending_results();

    void execute_tests();

private:
    template<typename ProxiedReturnType>
    using TestPropertyGetter = Glib::PropertyProxy_ReadOnly<ProxiedReturnType> (Test::*)() const;

    static const log4cxx::LoggerPtr area_logger;
    static const char *const area_name;

    static Glib::RefPtr<Gio::ListModel> test_group_expand(const Glib::RefPtr<Glib::ObjectBase> &item);

    // TODO: move to a location for all areas
    static std::optional<std::pair<const std::optional<Test>&, Gtk::Label *>> bind_helper(
        const Glib::RefPtr<Gtk::ListItem> &list_item)
    {
        const auto requirement = std::dynamic_pointer_cast<Requirement>(list_item->get_item());

        if (requirement == nullptr)
            return std::nullopt;

        const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());

        if (label == nullptr)
            return std::nullopt;

        return std::make_pair(std::cref(requirement->observe_test()), label);
    }

    // TODO: move to a location for all areas
    template<typename GetterReturnType>
    static void on_bind_test_property(const Glib::RefPtr<Gtk::ListItem> &list_item,
            sigc::mem_functor<TestPropertyGetter<GetterReturnType>> property_functor)
    {
        const auto widgets = bind_helper(list_item);

        if (widgets.has_value()) {
            if (widgets->first.has_value() && widgets->second != nullptr) {
                Glib::Binding::bind_property(
                    property_functor.operator()(*widgets->first),
                    widgets->second->property_label(),
                    Glib::Binding::Flags::SYNC_CREATE
                );
            }
        }
    }

    // TODO: move to a location for all areas
    template<typename GetterReturnType> requires mp_helpers::is_optional<GetterReturnType>::value
    static void on_bind_test_property_opt(const Glib::RefPtr<Gtk::ListItem> &list_item,
            sigc::mem_functor<TestPropertyGetter<GetterReturnType>> property_functor)
    {
        const auto widgets = bind_helper(list_item);

        if (widgets.has_value()) {
            if (widgets->first.has_value() && widgets->second != nullptr) {
                Glib::Binding::bind_property(
                    property_functor.operator()(*widgets->first),
                    widgets->second->property_label(),
                    Glib::Binding::Flags::SYNC_CREATE,
                    [label = widgets->second](const GetterReturnType& from) -> std::optional<Glib::ustring>
                    {
                        if (from.has_value())
                            return *from;

                        // TODO apply CSS styling here.
                        std::ignore = label;
                        // https://github.com/oliverdixon/OptiFOL-Software/blob/fd644972cbadb41c92e86098f7c05dca999743fd/Source/GUI/Panels/RequirementsIndexArea.cpp#L382
                        return "Empty";
                    }
                );
            }
        }
    }

    std::pair<Gtk::Widget *, Gtk::Widget *> on_off_widgets;
    Gtk::ColumnView *const test_groups_view;
    Gtk::Button *const run_tests_button;
    const Glib::RefPtr<Gtk::TextBuffer> run_tests_output_buffer;

    Glib::RefPtr<const Subsystem> active_subsystem;
    Glib::RefPtr<Gio::ListStore<TestGroup>> data_model;
    Glib::RefPtr<Gtk::SingleSelection> selection_model = Gtk::SingleSelection::create();
    Glib::RefPtr<Gtk::TreeListModel> tree_model;

    std::unique_ptr<TestListenerBase> test_listener;
    std::optional<ProcessExecutor> test_executor;

    std::unordered_map<TestResult *, std::shared_ptr<TestResult>, std::hash<TestResult>,
            DereferencingEqualityFunctor<const TestResult *, const TestResult>>
            received_test_results;
};

} // namespace optifol

#endif // TESTINGAREA_HPP
