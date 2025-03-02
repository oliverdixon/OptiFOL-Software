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

#include "MainWindow.hpp"

#include "GTKHelpers.hpp"
#include "../Exceptions/BadStorageNotificationException.hpp"
#include "../Exceptions/StorageConnectionException.hpp"
#include "../Storage/PGDatabaseController.hpp"

namespace optifol
{

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
    }

    const auto selection_model = Gtk::SingleSelection::create(storage->get_project_model());
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    project_view->set_model(selection_model);

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(&MainWindow::on_setup_label);
    factory->signal_bind().connect(sigc::mem_fun(*this, &MainWindow::on_bind_name));
    project_view->set_factory(factory);
}

void MainWindow::on_setup_label(const Glib::RefPtr<Gtk::ListItem> &item)
{
    item->set_child(*Gtk::make_managed<Gtk::Label>("", Gtk::Align::START));
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

void MainWindow::on_bind_name(const Glib::RefPtr<Gtk::ListItem> &item) const
{
    const auto position = item->get_position();

    if (position != GTK_INVALID_LIST_POSITION) {
        const auto label = dynamic_cast<Gtk::Label*>(item->get_child());
        if (label != nullptr) {
            const auto project = storage->get_project_model()->get_item(position);
            label->set_text(project->get_identifier());
        }
    }
}

}
