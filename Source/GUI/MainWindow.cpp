/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Optifol GTK main window
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#include "MainWindow.hpp"

#include "GTKHelpers.hpp"
#include "../Exceptions/StorageConnectionException.hpp"
#include "Panels/ProjectHierarchyPane.hpp"
#include "Panels/RequirementsIndexArea.hpp"

namespace optifol
{

const char * const MainWindow::area_name = "Main Window";

MainWindow::MainWindow():
    builder(Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow.ui")),
    root_grid(GTKHelpers::get_widget<Gtk::Box>(area_name, *builder, "root_grid")),
    database_alert(GTKHelpers::get_object<Gtk::AlertDialog>(area_name, *builder, "database_alert"))
{
    set_title("Optifol");
    set_default_size(600, 400);
    set_child(*root_grid);

    try {
        requirements_index_area = std::make_unique<RequirementsIndexArea>(*builder);
        project_hierarchy_pane = std::make_unique<ProjectHierarchyPane>(
            *builder,
            Gio::ListStore<Project>::create(),
            sigc::mem_fun(*requirements_index_area, &RequirementsIndexArea::select_model),
            sigc::mem_fun(*requirements_index_area, &RequirementsIndexArea::deselect_model)
        );
        analysis_area = std::make_unique<AnalysisArea>(*builder);
    } catch (const StorageConnectionException& exception) {
        database_alert->set_detail(exception.what());
        database_alert->show(*this);
        return;
    }

    const auto css_provider = Gtk::CssProvider::create();
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    css_provider->load_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow/styles.css");
}

}
