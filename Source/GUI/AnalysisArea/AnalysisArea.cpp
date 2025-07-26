/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Analysis and Optimisation view in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-04-27
 * @version Development
 */

#include "AnalysisArea.hpp"

#include "../../Storage/Subsystem.hpp"
#include "../GTKHelpers.hpp"
#include "../RequirementsIndexArea/RequirementsIndexArea.hpp"

namespace optifol
{

const char * const AnalysisArea::area_name = "Analysis and Optimisation Area";

AnalysisArea::AnalysisArea(Gtk::Builder &builder) :
    on_off_widgets(
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "analysis_advice_unselected"),
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "analysis_index_content")
    ),
    groups_view(GTKHelpers::get_widget<Gtk::ColumnView>(area_name, builder, "analysis_groups_view")),
    context_menu(
        groups_view,
        GTKHelpers::get_object<Gio::Menu>(area_name, builder, "analysis_groups_context_menu"),
        {
            {
                "new_analysis_group",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "new_analysis_group"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_analysis_group_popover"),
                true
            },
            {
                "edit_analysis_group",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "edit_analysis_group"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "edit_analysis_group_popover"),
                false
            },
            {
                "delete_analysis_group",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "delete_analysis_group"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "delete_analysis_group_popover"),
                false
            }
        }
    ),
    new_analysis_group_popover(builder, *this)
{
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);

    selection_model->signal_selection_changed().connect([this](guint, const guint n_items)
    {
        if (n_items == 0 || selection_model->get_selected() == 0) {
            context_menu.disable_action("edit_analysis_group");
            context_menu.disable_action("delete_analysis_group");
        } else {
            context_menu.enable_action("edit_analysis_group");
            context_menu.enable_action("delete_analysis_group");
        }
    });

    selection_model->signal_items_changed().connect([this](guint, const guint removed, guint)
    {
        if (removed > 0) {
            // If anything was removed from the model, just deselect everything out of an abundance of caution.
            context_menu.disable_action("edit_analysis_group");
            context_menu.disable_action("delete_analysis_group");
        }
    });

    groups_view->set_model(selection_model);

    const auto columns = groups_view->get_columns();
    const auto column_count = columns->get_n_items();

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto& gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "analysis_requirement_name") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_expandable_label, false));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                {
                    /*
                     * Cannot use sigc::bind on non-constant fixed arguments, as any fixed argument values are evaluated
                     * on execution of the bind call. Hence, calling bind(tree_model) on the below function would fix
                     * all subsequent invocations to the initial value of the tree_model.
                     */
                    StorageObjectBase::bind_name_property_expandable(list_item, tree_model);
                });

            } else if (gtk_id == "analysis_requirement_statement") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, true));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_statement));

            } else if (gtk_id == "analysis_requirement_cnf") {

                factory->signal_setup().connect(sigc::bind(&GTKHelpers::setup_label, true));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_normalised));

            } else

                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
        }
    }
}

void AnalysisArea::select_model(const Glib::RefPtr<Subsystem> &subsystem_model)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);

    active_subsystem = subsystem_model;
    tree_model = Gtk::TreeListModel::create(
        active_subsystem->get_analysis_groups(),
        sigc::ptr_fun(&AnalysisGroup::get_expanded_list<AnalysisGroup>),
        true,
        true
    );
    selection_model->set_model(tree_model);
}

void AnalysisArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);

    active_subsystem = nullptr;
    tree_model = nullptr;
    selection_model->set_model(nullptr);
}

Subsystem *AnalysisArea::observe_active_subsystem() noexcept
{
    return active_subsystem.get();
}

const Subsystem *AnalysisArea::observe_active_subsystem() const noexcept
{
    return active_subsystem.get();
}

} // namespace optifol
