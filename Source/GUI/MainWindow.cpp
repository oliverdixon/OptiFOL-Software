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
#include "Panels/ProjectHierarchyPane.hpp"
#include "Panels/RequirementsIndexArea.hpp"

namespace optifol
{

MainWindow::MainWindow():
    builder(Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow.ui")),
    root_grid(GTKHelpers::get_widget<Gtk::Box>("Main Window", builder, "root_grid")),
    project_view(GTKHelpers::get_widget<Gtk::ListView>("Main Window", builder, "project_view")),
    update_storage_button(GTKHelpers::get_widget<Gtk::Button>("Main Window", builder, "update_storage_button")),
    database_alert(GTKHelpers::get_object<Gtk::AlertDialog>("Main Window", builder, "database_alert"))
{
    set_title("OptiFOL");
    set_default_size(600, 400);
    set_child(*root_grid);

    try {
        storage = std::make_unique<PGDatabaseController>("postgresql://owd@localhost/optifol");

        requirements_index_area = std::make_unique<RequirementsIndexArea>(
            GTKHelpers::get_widget<Gtk::ColumnView>("Main Window", builder, "requirements_view"),
            GTKHelpers::get_widget<Gtk::Widget>("Main Window", builder, "requirements_index_advice_unselected"),
            GTKHelpers::get_widget<Gtk::Widget>("Main Window", builder, "requirements_index_content")
        );

        project_hierarchy_pane = std::make_unique<ProjectHierarchyPane>(
            project_view,
            Glib::make_refptr_for_instance(new ProjectHierarchicalModel()), // TODO
            sigc::mem_fun(*requirements_index_area, &RequirementsIndexArea::select_model),
            sigc::mem_fun(*requirements_index_area, &RequirementsIndexArea::deselect_model)
        );
    } catch (const StorageConnectionException& exception) {
        database_alert->set_detail(exception.what());
        database_alert->show(*this);
        return;
    }

    const auto css_provider = Gtk::CssProvider::create();
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    css_provider->load_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow/styles.css");

    update_storage_button->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_update_storage));
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

}
