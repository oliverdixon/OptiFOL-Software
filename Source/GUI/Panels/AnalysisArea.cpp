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

#include "RequirementsIndexArea.hpp"
#include "../GTKHelpers.hpp"

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
    )
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
    guint processed_columns = 0;

    for (guint position = 0; position < column_count; ++position) {
        Glib::RefPtr<Gtk::ColumnViewColumn> column = nullptr;

        if ((column = columns->get_typed_object<Gtk::ColumnViewColumn>(position)) != nullptr) {
            const auto& gtk_id = column->get_id();
            const auto factory = Gtk::SignalListItemFactory::create();

            if (gtk_id == "analysis_requirement_name") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_expandable_label, false));
                factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem> &list_item)
                {
                    /*
                     * Cannot use sigc::bind on non-constant fixed arguments, as any fixed argument values are evaluated
                     * on execution of the bind call. Hence, calling bind(tree_model) on the below function would fix
                     * all subsequent invocations to the initial value of the tree_model.
                     */
                    GTKHelpers::on_bind_expandable_name(list_item, tree_model);
                });
            } else if (gtk_id == "analysis_requirement_statement") {
                factory->signal_setup().connect(sigc::bind(&GTKHelpers::on_setup_flat_label, false));
                factory->signal_bind().connect(sigc::ptr_fun(&RequirementsIndexArea::on_bind_property_statement));
            } else
                // Jump out here if unrecognised, so all further code can assume a factory was configured.
                continue;

            column->set_factory(factory);
            ++processed_columns;
        }
    }

    configure_new_analysis_group_popover(builder);
    configure_delete_analysis_group_popover(builder);
}

void AnalysisArea::select_model(const Glib::RefPtr<Gio::ListStore<AnalysisGroup>> &new_model)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);

    data_model = new_model;
    tree_model = Gtk::TreeListModel::create(data_model, sigc::ptr_fun(&AnalysisArea::analysis_group_expand), true,
        true);
    selection_model->set_model(tree_model);
}

void AnalysisArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);

    tree_model = nullptr;
    data_model = nullptr;
    selection_model->set_model(nullptr);
}

void AnalysisArea::configure_new_analysis_group_popover(Gtk::Builder &builder) const
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_analysis_group_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder,
        "new_analysis_group_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_analysis_group_cancel");
    const auto property_name = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder,
        "new_analysis_group_property_name");

    cancel_button->signal_clicked().connect([popover, property_name]
    {
        popover->popdown();
        property_name->set_text("");
    });

    confirm_button->signal_clicked().connect([this, popover, property_name]
    {
        popover->popdown();
        data_model->append(Glib::make_refptr_for_instance(new AnalysisGroup(property_name->get_text())));
    });
}

void AnalysisArea::configure_delete_analysis_group_popover(Gtk::Builder &builder) const
{
    // TODO
}

Glib::RefPtr<Gio::ListModel> AnalysisArea::analysis_group_expand(const Glib::RefPtr<Glib::ObjectBase> &item)
{
    const auto candidate = std::dynamic_pointer_cast<AnalysisGroup>(item);

    if (candidate != nullptr)
        return candidate->requirements;

    return nullptr;
}

}
