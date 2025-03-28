/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Project Hierarchical view in the OptiFOL Main Window
 * @author Oliver Dixon
 * @date 2025-03-27
 * @version Development
 */

#include "ProjectHierarchyPane.hpp"

#include "../Logging.hpp"
#include "../Storage/IStorageObject.hpp"

namespace optifol
{

std::shared_ptr<log4cxx::Logger> ProjectHierarchyPane::logger(log4cxx::Logger::getLogger("OptiFOL"));

ProjectHierarchyPane::ProjectHierarchyPane(Gtk::ListView *view, const Glib::RefPtr<PGProjectModel>& initial_model) :
    project_model(initial_model)
{
    tree_model = Gtk::TreeListModel::create(initial_model,
        sigc::mem_fun(*this, &ProjectHierarchyPane::on_expand), true, true);
    const auto selection_model = Gtk::SingleSelection::create(tree_model);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    view->set_model(selection_model);

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

Glib::RefPtr<Gio::ListModel> ProjectHierarchyPane::on_expand(
        const Glib::RefPtr<Glib::ObjectBase> &item) const
{
    const auto project_candidate = std::dynamic_pointer_cast<Project>(item);
    if (project_candidate != nullptr)
        return project_model->get_subsystem_model(project_candidate);

    return nullptr;
}

}
