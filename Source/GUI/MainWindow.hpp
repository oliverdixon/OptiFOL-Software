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
    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::AboutDialog * dialog;
};

}

#endif //OPTIFOL_MAINWINDOW_HPP
