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
#include <log4cxx/logger.h>

#include "../Storage/PGDatabaseController.hpp"

namespace optifol
{

class MainWindow :
        public Gtk::ApplicationWindow
{
public:
    MainWindow();

private:
    static void on_setup_storable_model(const Glib::RefPtr<Gtk::ListItem> &item);

    void on_bind_storable_label(const Glib::RefPtr<Gtk::ListItem>& item) const;

    void on_update_storage();

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::ScrolledWindow * scrolled_window;
    Gtk::ListView * project_view;
    Gtk::Grid * root_grid;
    Gtk::Button * update_storage_button;

    Glib::RefPtr<Gtk::AlertDialog> database_alert;
    Glib::RefPtr<Gtk::TreeListModel> tree_list_model;

    std::unique_ptr<IStorageController> storage;
    static std::shared_ptr<log4cxx::Logger> logger;
};

}

#endif
