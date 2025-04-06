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

const char * const MainWindow::area_name = "Main Window";

MainWindow::MainWindow():
    builder(Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow.ui")),
    root_grid(GTKHelpers::get_widget<Gtk::Box>("area_name", *builder, "root_grid")),
    database_alert(GTKHelpers::get_object<Gtk::AlertDialog>("area_name", *builder, "database_alert"))
{
    set_title("OptiFOL");
    set_default_size(600, 400);
    set_child(*root_grid);

    try {
        storage = std::make_unique<PGDatabaseController>("postgresql://owd@localhost/optifol");
        requirements_index_area = std::make_unique<RequirementsIndexArea>(*builder);
        project_hierarchy_pane = std::make_unique<ProjectHierarchyPane>(
            *builder,
            storage->build_project_hierarchical_model(),
            sigc::mem_fun(*requirements_index_area, &RequirementsIndexArea::select_model),
            sigc::mem_fun(*requirements_index_area, &RequirementsIndexArea::deselect_model),
            sigc::mem_fun(*this, &MainWindow::on_update_storage)
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
