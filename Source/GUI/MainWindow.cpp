/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the OptiFOL GTK main window
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#include <log4cxx/basicconfigurator.h>

#include "MainWindow.hpp"

#include "GTKHelpers.hpp"
#include "../Exceptions/BadStorageNotificationException.hpp"
#include "../Exceptions/StorageConnectionException.hpp"
#include "../Storage/PGDatabaseController.hpp"

namespace optifol
{

std::shared_ptr<log4cxx::Logger> MainWindow::logger(log4cxx::Logger::getLogger("OptiFOL"));

MainWindow::MainWindow():
    builder(Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow.ui")),
    scrolled_window(GTKHelpers::get_widget<Gtk::ScrolledWindow>("Main Window", builder, "scrolled_window")),
    project_view(GTKHelpers::get_widget<Gtk::ListView>("Main Window", builder, "project_view")),
    root_grid(GTKHelpers::get_widget<Gtk::Grid>("Main Window", builder, "root_grid")),
    update_storage_button(GTKHelpers::get_widget<Gtk::Button>("Main Window", builder, "update_storage_button")),
    database_alert(GTKHelpers::get_object<Gtk::AlertDialog>("Main Window", builder, "database_alert"))
{
    set_title("OptiFOL");
    set_default_size(600, 400);
    set_child(*scrolled_window);

    try {
        storage = std::make_unique<PGDatabaseController>("postgresql://owd@localhost/optifol");
        update_storage_button->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_update_storage));
    } catch (const StorageConnectionException& exception) {
        database_alert->set_detail(exception.what());
        database_alert->show(*this);
        return;
    }

    tree_list_model = Gtk::TreeListModel::create(storage->peek_project_model(),
        sigc::mem_fun(*this, &MainWindow::on_expand_storable_label), true, true);
    const auto selection_model = Gtk::SingleSelection::create(tree_list_model);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    project_view->set_model(selection_model);

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(sigc::ptr_fun(MainWindow::on_setup_storable_model));
    factory->signal_bind().connect(sigc::mem_fun(*this, &MainWindow::on_bind_storable_label));
    project_view->set_factory(factory);
}

void MainWindow::on_setup_storable_model(const Glib::RefPtr<Gtk::ListItem> &item)
{
    const auto expander = Gtk::make_managed<Gtk::TreeExpander>();
    const auto label = Gtk::make_managed<Gtk::Label>();

    label->set_halign(Gtk::Align::START);
    expander->set_child(*label);
    item->set_child(*expander);
}

void MainWindow::on_update_storage()
{
    try {
        storage->update();
    } catch (const BadStorageNotificationException& exception) {
        database_alert->set_detail(exception.what());
        database_alert->show(*this);
    }
}

void MainWindow::on_bind_storable_label(const Glib::RefPtr<Gtk::ListItem> &item) const
{
    const auto position = item->get_position();

    if (position == GTK_INVALID_LIST_POSITION) {
        LOG4CXX_WARN(logger, "Invalid position " << std::to_string(position) << " selected in the project view.");
        return;
    }

    const auto gui_row = tree_list_model->get_row(position);
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

Glib::RefPtr<Gio::ListModel> MainWindow::on_expand_storable_label(const Glib::RefPtr<Glib::ObjectBase> &item) const
{
    const auto project_candidate = std::dynamic_pointer_cast<Project>(item);
    if (project_candidate != nullptr)
        return storage->expand_project_model(project_candidate);

    return nullptr;
}

}
