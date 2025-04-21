/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Optifol GTK Application
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#ifndef OPTIFOL_APPLICATION_HPP
#define OPTIFOL_APPLICATION_HPP

#include <gtkmm.h>

namespace optifol
{

class MainWindow;

class Application :
        public Gtk::Application
{
public:
    static Glib::RefPtr<Application> create();

protected:
    Application();

    void on_startup() override;

    void on_activate() override;

private:
    MainWindow* create_main_window();

    void show_about_dialog() const;

    static void open_application_help();

    void quit_application();

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::AboutDialog * about_dialog = nullptr;
};

}

#endif
