//
// Created by owd on 18/01/25.
//

#include <iostream>

#include "Application.hpp"
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
    Glib::set_application_name("OptiFOL");
}

void Application::on_activate()
{
    const auto main_window = create_main_window();
    main_window->present();

    // ReSharper disable once CppDFAMemoryLeak
}

MainWindow * Application::create_main_window()
{
    // ReSharper disable once CppDFAMemoryLeak
    const auto main_window = new MainWindow();
    add_window(*main_window);

    main_window->signal_hide().connect([main_window](){ delete main_window; });

    return main_window;
}

}
