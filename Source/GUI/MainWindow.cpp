/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "MainWindow.hpp"

#include <iostream>

#include "GTKHelpers.hpp"
#include "../Exceptions/StorageConnectionException.hpp"
#include "../Storage/PGDatabase.hpp"

namespace optifol
{

MainWindow::MainWindow():
    builder(Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow.ui")),
    scrolled_window(GTKHelpers::get_widget<Gtk::ScrolledWindow>("Main Window", builder, "scrolled_window")),
    directory_view(GTKHelpers::get_widget<Gtk::ListView>("Main Window", builder, "directory_view")),
    root_grid(GTKHelpers::get_widget<Gtk::Grid>("Main Window", builder, "root_grid")),
    database_alert(GTKHelpers::get_object<Gtk::AlertDialog>("Main Window", builder, "database_alert"))
{
    set_title("OptiFOL");
    set_default_size(600, 400);
    set_child(*scrolled_window);

    const auto selection_model = Gtk::SingleSelection::create(directory_model);
    selection_model->set_autoselect(false);
    selection_model->set_can_unselect(true);
    directory_view->set_model(selection_model);

    const auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(&MainWindow::on_setup_label);
    factory->signal_bind().connect(sigc::mem_fun(*this, &MainWindow::on_bind_name));
    directory_view->set_factory(factory);

    try {
        const std::unique_ptr<IStorageController> storage =
            std::make_unique<PGDatabase>("postgresql://owd@localhost/optifol");

        const auto project_names = storage->get_project_names();
        for (const auto& name : project_names)
            std::cout << name << std::endl; // TODO: just for testing!
    } catch (const StorageConnectionException& exception) {
        database_alert->set_detail(exception.what());
        database_alert->show(*this);
    }
}

void MainWindow::on_setup_label(const Glib::RefPtr<Gtk::ListItem> &item)
{
    item->set_child(*Gtk::make_managed<Gtk::Label>("", Gtk::Align::START));
}

void MainWindow::on_bind_name(const Glib::RefPtr<Gtk::ListItem> &item) const
{
    const auto pos = item->get_position();

    if (pos != GTK_INVALID_LIST_POSITION) {
        const auto label = dynamic_cast<Gtk::Label*>(item->get_child());
        if (label != nullptr) {
            const auto info = directory_model->get_typed_object<Gio::FileInfo>(pos);
            if (info != nullptr) {
                const auto& prefix = directory_model->get_file()->get_path();
                label->set_text(prefix + '/' + info->get_name());
            }
        }
    }
}

}
