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
    model(initial_model)
{
    const auto view = GTKHelpers::get_widget<Gtk::ListView>(area_name, builder, "project_view");

    signal_select_subsystem.connect(selected_subsystem_callback);
    signal_deselect_subsystem.connect(deselected_subsystem_callback);

    tree_model = Gtk::TreeListModel::create(model, sigc::mem_fun(*this, &ProjectHierarchyPane::on_expand), true,
        true);
    stack_switcher->property_selected().signal_changed().connect(sigc::mem_fun(*this,
        &ProjectHierarchyPane::on_dropdown_changed));

    const auto new_project_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "new_project_button");
    new_project_button->signal_clicked().connect([this]
        { model->append(Glib::make_refptr_for_instance(new Project("New Project"))); });

    const auto selection_model = Gtk::SingleSelection::create(tree_model);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    view->set_model(selection_model);

    const auto new_subsystem_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder,
        "new_subsystem_button");
    new_subsystem_button->signal_clicked().connect([selection_model]
    {
        const auto subsystem_model = selection_model->get_model()->
            get_typed_object<Project>(selection_model->get_selected())->subsystems;

        subsystem_model->append(Glib::make_refptr_for_instance(new Subsystem("New Subsystem")));
    });

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(sigc::ptr_fun(ProjectHierarchyPane::on_setup));
    factory->signal_bind().connect(sigc::mem_fun(*this, &ProjectHierarchyPane::on_bind));
    view->set_factory(factory);
}

void ProjectHierarchyPane::on_setup(const Glib::RefPtr<Gtk::ListItem> &item)
{
    const auto expander = Gtk::make_managed<Gtk::TreeExpander>();
    const auto label = Gtk::make_managed<Gtk::Label>();

    label->set_halign(Gtk::Align::START);
    expander->set_child(*label);
    item->set_child(*expander);
}

void ProjectHierarchyPane::on_bind(const Glib::RefPtr<Gtk::ListItem> &item) const
{
    const auto position = item->get_position();

    if (position == GTK_INVALID_LIST_POSITION) {
        LOG4CXX_WARN(logger, "Invalid position " << std::to_string(position) << " selected in the project view.");
        return;
    }

    const auto gui_row = tree_model->get_row(position);
    if (!gui_row) {
        LOG4CXX_WARN(logger, "No row at selected position " << std::to_string(position) << " in the project view.");
        return;
    }

    const auto model_item = std::dynamic_pointer_cast<IStorageObject>(gui_row->get_item());
    if (!model_item) {
        LOG4CXX_WARN(logger, "Unexpected type of entity at row " << std::to_string(position) <<
            " in the project view.");
        return;
    }

    const auto expander = dynamic_cast<Gtk::TreeExpander*>(item->get_child());
    if (!expander) {
        LOG4CXX_WARN(logger, "Unexpected type of expander widget at row " << std::to_string(position) <<
            " in the project view.");
        return;
    }

    expander->set_list_row(gui_row);

    const auto label = dynamic_cast<Gtk::Label*>(expander->get_child());
    if (!label) {
        LOG4CXX_WARN(logger, "Unexpected type of label in the expander for the row at " << std::to_string(position) <<
            " in the project view.");
        return;
    }

    label->set_text(model_item->get_identifier());
}

void ProjectHierarchyPane::on_dropdown_changed() const
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

Glib::RefPtr<Gio::ListModel> ProjectHierarchyPane::on_expand(
        const Glib::RefPtr<Glib::ObjectBase> &item) const
{
    const auto project_candidate = std::dynamic_pointer_cast<Project>(item);

    if (project_candidate != nullptr) {
        // Find the project being expanded in the model
        const auto [found, position] = model->find(project_candidate);
        if (found)
            return model->get_item(position)->subsystems;
    } else {
        const auto subsystem_candidate = std::dynamic_pointer_cast<Subsystem>(item);
        if (subsystem_candidate != nullptr) {
            // Find the subsystem being expanded in the model, noting that subsystems can be nested to arbitrary levels
            // TODO: recurse down the tree; provide a way of calculating the index.
            // https://github.com/oliverdixon/OptiFOL-Software/blob/e991dbf40855841ac5e0c6dafdea4df218c1dcd5/Source/GUI/Panels/ProjectHierarchyPane.cpp#L113
        }
    }

    return nullptr;
}

}
