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
#include "ProjectHierarchyPane.hpp"
#include "../Exceptions/BadStorageNotificationException.hpp"
#include "../Exceptions/StorageConnectionException.hpp"
#include "../Storage/PGDatabaseController.hpp"

namespace optifol
{

MainWindow::MainWindow():
    builder(Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow.ui")),
    root_grid(GTKHelpers::get_widget<Gtk::Box>("Main Window", builder, "root_grid")),
    update_storage_button(GTKHelpers::get_widget<Gtk::Button>("Main Window", builder, "update_storage_button")),
    database_alert(GTKHelpers::get_object<Gtk::AlertDialog>("Main Window", builder, "database_alert"))
{
    set_title("OptiFOL");
    set_default_size(600, 400);
    set_child(*root_grid);

    try {
        storage = std::make_unique<PGDatabaseController>("postgresql://owd@localhost/optifol");
        update_storage_button->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_update_storage));

        project_hierarchy_pane = std::make_unique<ProjectHierarchyPane>(
            GTKHelpers::get_widget<Gtk::ListView>("Main Window", builder, "project_view"),
            storage->peek_project_model()
        );

        const auto test_model = storage->peek_project_model()->get_subsystem_model(218)->get_requirement_model(547);
        test_model->enqueue_load(1);
        test_model->load();

        requirements_index_area = std::make_unique<RequirementsIndexArea>(
            GTKHelpers::get_widget<Gtk::ColumnView>("Main Window", builder, "requirements_view"),
            test_model
        );
    } catch (const StorageConnectionException& exception) {
        database_alert->set_detail(exception.what());
        database_alert->show(*this);
        return;
    }

    const auto css_provider = Gtk::CssProvider::create();
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    css_provider->load_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow/styles.css");
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
