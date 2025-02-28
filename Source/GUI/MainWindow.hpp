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

#include "../Storage/IStorageController.hpp"

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

    void on_bind_name(const Glib::RefPtr<Gtk::ListItem>& item) const;

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::ScrolledWindow * scrolled_window;
    Gtk::ListView * directory_view;
    Gtk::Grid * root_grid;
    Gtk::Button * update_storage_button;

    Glib::RefPtr<Gtk::AlertDialog> database_alert;
    std::unique_ptr<IStorageController> storage;

    Glib::RefPtr<Gtk::DirectoryList> directory_model =
        Gtk::DirectoryList::create("standard::name", Gio::File::create_for_path("/home/owd/"));
};

}

#endif
