/*
 * Copyright (c) All Rights Reserved
 * 2024-2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 18/01/25.
//

#ifndef OPTIFOL_MAINWINDOW_HPP
#define OPTIFOL_MAINWINDOW_HPP

#include <gtkmm.h>

namespace optifol
{

class MainWindow :
        public Gtk::ApplicationWindow
{
public:
    MainWindow();

private:
    static void on_setup_label(const Glib::RefPtr<Gtk::ListItem> &item);

    void on_bind_name(const Glib::RefPtr<Gtk::ListItem>& item) const;

    Gtk::ScrolledWindow * scrolled_window;
    Gtk::ListView * directory_view;

    Glib::RefPtr<Gtk::DirectoryList> directory_model =
        Gtk::DirectoryList::create("standard::name", Gio::File::create_for_path("/home/owd/"));

    Glib::RefPtr<Gtk::Builder> builder;
};

}

#endif //OPTIFOL_MAINWINDOW_HPP
