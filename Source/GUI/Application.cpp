/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Optifol GTK Application
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#include <iostream>

#include "../Storage/Subsystem.hpp"
#include "Application.hpp"
#include "GTKHelpers.hpp"
#include "MainWindow.hpp"

namespace optifol
{

Glib::RefPtr<Application> Application::create()
{
    return Glib::make_refptr_for_instance<Application>(new Application());
}

Application::Application() :
    Gtk::Application("uk.ac.york.www_users.od641.optifol")
{
    Glib::set_application_name("Optifol");
}

void Application::on_startup()
{
    Gtk::Application::on_startup();

    add_action("new", [] { std::cout << "New\n"; });
    add_action("open", [] { std::cout << "Open\n"; });
    add_action("quit", sigc::mem_fun(*this, &Application::quit_application));
    add_action("about", sigc::mem_fun(*this, &Application::show_about_dialog));
    add_action("help", &Application::open_application_help);

    builder = Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/Application.ui");
    set_menubar(GTKHelpers::get_object<Gio::Menu>("Application Root", *builder, "top_menu"));
    about_dialog = GTKHelpers::get_widget<Gtk::AboutDialog>("Application Root", *builder, "about_dialog");
    about_dialog->set_hide_on_close();
}

void Application::on_activate()
{
    const auto main_window = create_main_window();
    main_window->present();
    about_dialog->set_transient_for(*main_window);

    // ReSharper disable once CppDFAMemoryLeak
}

MainWindow *Application::create_main_window()
{
    // Memory leak warning here is a false positive, as windows are managed by the GTK management engine
    // ReSharper disable once CppDFAMemoryLeak
    const auto main_window = new MainWindow();
    add_window(*main_window);

    main_window->set_show_menubar();
    main_window->signal_hide().connect([main_window]() { delete main_window; });

    return main_window;
}

void Application::show_about_dialog() const
{
    about_dialog->set_visible();
    about_dialog->present();
}

void Application::open_application_help()
{
    Gio::AppInfo::launch_default_for_uri("https://www-users.york.ac.uk/~od641");
}

void Application::quit_application()
{
    quit();

    // Destruct all windows constituting the Application instance.
    for (const auto windows = get_windows(); const auto window: windows)
        window->set_visible(false);
}

} // namespace optifol
