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
#include "../Storage/Project.hpp"

namespace optifol
{

class MainWindow :
        public Gtk::ApplicationWindow
{
public:
    MainWindow();

private:
    static void on_setup_label(const Glib::RefPtr<Gtk::ListItem> &item);

    void on_update_storage();

    void on_bind_project(const Glib::RefPtr<Gtk::ListItem>& item) const;

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::ScrolledWindow * scrolled_window;
    Gtk::ListView * project_view;
    Gtk::Grid * root_grid;
    Gtk::Button * update_storage_button;

    Glib::RefPtr<Gtk::AlertDialog> database_alert;

    /*
     * TODO: we should be using IStorageController here, but I had to change it to the concrete type temporarily because
     *  we need (more or less) direct access to the models, which are not currently storage-agnostic. This needs to be
     *  fixed: there should be a PGStorableObjectModel, as there currently is, but most of the stuff in there should be
     *  moved to a non-PG interface. Really only the constructor needs to be PG-specific; the rest can be in a
     *  controller-agnostic base class. Then we can provide observers for the models on IStorageController.
     */
    std::unique_ptr<PGDatabaseController> storage;
};

}

#endif
