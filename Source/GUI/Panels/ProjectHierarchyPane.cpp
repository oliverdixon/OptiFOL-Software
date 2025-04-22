/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Project Hierarchical view in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#include "ProjectHierarchyPane.hpp"

#include <iostream>

#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../Storage/IStorageObject.hpp"
#include "../Storage/Subsystem.hpp"

namespace optifol
{

std::shared_ptr<log4cxx::Logger> ProjectHierarchyPane::logger(log4cxx::Logger::getLogger("Optifol"));

const char * const ProjectHierarchyPane::area_name = "Project Pane Area";

ProjectHierarchyPane::ProjectHierarchyPane(Gtk::Builder &builder,
        const Glib::RefPtr<Gio::ListStore<Project>> &initial_model,
        sigc::slot<SelectedCallbackSignature> &&selected_subsystem_callback,
        sigc::slot<DeselectedCallbackSignature> &&deselected_subsystem_callback) :
    stack_switcher(GTKHelpers::get_widget<Gtk::DropDown>(area_name, builder, "project_pane_switcher")),
    stack(GTKHelpers::get_widget<Gtk::Stack>(area_name, builder, "project_pane_stack")),
    view(GTKHelpers::get_widget<Gtk::ListView>(area_name, builder, "project_view")),
    root_model(initial_model),
    context_menu(view, GTKHelpers::get_object<Gio::Menu>(area_name, builder, "structure_context_menu"),
                 sigc::mem_fun(*this, &ProjectHierarchyPane::new_project),
                 sigc::mem_fun(*this, &ProjectHierarchyPane::new_subsystem),
                 sigc::mem_fun(*this, &ProjectHierarchyPane::edit_structure),
                 sigc::mem_fun(*this, &ProjectHierarchyPane::delete_structure))
{
    view->signal_activate().connect(sigc::mem_fun(*this, &ProjectHierarchyPane::switch_subsystem));

    signal_select_subsystem.connect(selected_subsystem_callback);
    signal_deselect_subsystem.connect(deselected_subsystem_callback);

    tree_model = Gtk::TreeListModel::create(root_model, sigc::ptr_fun(&ProjectHierarchyPane::tree_node_expand), true,
        true);
    stack_switcher->property_selected().signal_changed().connect(sigc::mem_fun(*this,
        &ProjectHierarchyPane::switch_visible_stack));

    selection_model = Gtk::SingleSelection::create(tree_model);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    view->set_model(selection_model);

    selection_model->signal_selection_changed().connect([this](guint, const guint n_items)
    {
        if (n_items == 0) {
            signal_deselect_subsystem();
            context_menu.disable_optional();
        } else
            context_menu.enable_optional();
    });

    selection_model->signal_items_changed().connect([this](guint, const guint removed, guint)
    {
        if (removed > 0) {
            // If anything was removed from the model, just deselect everything out of an abundance of caution.
            signal_deselect_subsystem();
            context_menu.disable_optional();
        }
    });

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(sigc::ptr_fun(ProjectHierarchyPane::tree_node_setup));
    factory->signal_bind().connect(sigc::mem_fun(*this, &ProjectHierarchyPane::tree_node_bind));
    view->set_factory(factory);
}

void ProjectHierarchyPane::tree_node_setup(const Glib::RefPtr<Gtk::ListItem> &item)
{
    const auto expander = Gtk::make_managed<Gtk::TreeExpander>();
    const auto label = Gtk::make_managed<Gtk::EditableLabel>();

    label->property_editing().signal_changed().connect([item]
    {
        tree_node_name_change(item);
    });

    label->set_halign(Gtk::Align::START);
    expander->set_child(*label);
    item->set_child(*expander);
}

void ProjectHierarchyPane::tree_node_bind(const Glib::RefPtr<Gtk::ListItem> &item) const
{
    const auto position = item->get_position();
    const auto model_item = std::dynamic_pointer_cast<IStorageObject>(item->get_item());
    const auto node_item = std::dynamic_pointer_cast<TreeNode>(item->get_item());
    const auto expander = dynamic_cast<Gtk::TreeExpander*>(item->get_child());

    if (position == GTK_INVALID_LIST_POSITION || model_item == nullptr || expander == nullptr || node_item == nullptr) {
        LOG4CXX_WARN(logger, "Invalid position " << std::to_string(position) << " selected in the project view.");
        return;
    }

    const auto gui_row = tree_model->get_row(position);
    if (!gui_row) {
        LOG4CXX_WARN(logger, "No row at selected position " << std::to_string(position) << " in the project view.");
        return;
    }

    expander->set_list_row(gui_row);

    const auto label = dynamic_cast<Gtk::EditableLabel*>(expander->get_child());
    if (!label) {
        LOG4CXX_WARN(logger, "Unexpected type of label in the expander for the row at " << std::to_string(position) <<
            " in the project view.");
        return;
    }

    label->set_text(model_item->get_identifier());
}

void ProjectHierarchyPane::switch_visible_stack() const
{
    switch (stack_switcher->get_selected()) {
    case static_cast<guint>(ProjectStackSwitcherIdx::Explorer):
        stack->set_visible_child("project_explorer");
        break;
    case static_cast<guint>(ProjectStackSwitcherIdx::Metadata):
        stack->set_visible_child("project_metadata");
        break;
    default:
        break;
    }
}

Glib::RefPtr<Gio::ListModel> ProjectHierarchyPane::tree_node_expand(
        const Glib::RefPtr<Glib::ObjectBase> &item)
{
    const auto candidate = std::dynamic_pointer_cast<TreeNode>(item);

    if (candidate != nullptr)
        return candidate->get_children();

    return nullptr;
}

void ProjectHierarchyPane::tree_node_name_change(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto item = std::dynamic_pointer_cast<IStorageObject>(list_item->get_item());
    const auto expander = dynamic_cast<Gtk::TreeExpander*>(list_item->get_child());

    if (item != nullptr && expander != nullptr) {
        const auto label = dynamic_cast<Gtk::EditableLabel*>(expander->get_child());
        if (label != nullptr)
            item->set_identifier(label->get_text());
    }
}

void ProjectHierarchyPane::new_project() const
{
    root_model->append(Glib::make_refptr_for_instance(new Project("New Project")));
}

void ProjectHierarchyPane::new_subsystem() const
{
    const auto candidate = std::dynamic_pointer_cast<const TreeNode>(
        selection_model->get_selected_item());

    if (candidate != nullptr)
        candidate->add("New Subsystem");
}

void ProjectHierarchyPane::edit_structure()
{

}

void ProjectHierarchyPane::delete_structure() const
{
    const auto candidate = std::dynamic_pointer_cast<TreeNode>(
        selection_model->get_selected_item());

    if (candidate != nullptr) {
        const auto owning_model = candidate->get_parent();
        if (owning_model != nullptr) {
            // If an owning model is explicitly declared, use that.
            const auto owning_model_count = owning_model->get_n_items();
            for (guint idx = 0; idx < owning_model_count; ++idx)
                if (owning_model->get_item(idx) == candidate)
                    owning_model->remove(idx);
        } else {
            // Otherwise, use the root model.
            const auto root_model_count = root_model->get_n_items();
            for (guint idx = 0; idx < root_model_count; ++idx)
                if (root_model->get_item(idx) == candidate)
                    root_model->remove(idx);
        }
    }

    signal_deselect_subsystem();
}

void ProjectHierarchyPane::switch_subsystem(guint) const
{
    const auto& candidate = selection_model->get_selected_item();

    const auto subsystem = std::dynamic_pointer_cast<const Subsystem>(candidate);
    if (subsystem != nullptr) {
        signal_select_subsystem(subsystem->requirements);
        return;
    }

    const auto project = std::dynamic_pointer_cast<const Project>(candidate);
    if (project != nullptr)
        signal_deselect_subsystem();
}

}
