/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the OptiFOL GTK main window
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#ifndef OPTIFOL_MAINWINDOW_HPP
#define OPTIFOL_MAINWINDOW_HPP

#include <gtkmm.h>

#include "../Storage/PGDatabaseController.hpp"
#include "Panels/ProjectHierarchyPane.hpp"
#include "Panels/RequirementsIndexArea.hpp"

namespace optifol
{

class MainWindow :
        public Gtk::ApplicationWindow
{
public:
    MainWindow();

private:
    void on_update_storage();

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::Box * const root_grid;
    Gtk::ListView * const project_view;
    Gtk::Button * const update_storage_button;

    Glib::RefPtr<Gtk::AlertDialog> database_alert;

    std::unique_ptr<ProjectHierarchyPane> project_hierarchy_pane;
    std::unique_ptr<RequirementsIndexArea> requirements_index_area;

    std::unique_ptr<IStorageController> storage;
};

}

#endif
