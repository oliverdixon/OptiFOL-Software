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

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <gtkmm.h>

namespace optifol
{

class MainWindow;

/**
 * @class Application
 * @brief The Optifol application managing startup, shutdown, the main window, and core user actions.
 */
class Application : public Gtk::Application
{
public:
    /**
     * @brief Create a new Application from a static context, producing a reference to the created Application in the
     *  GTK memory model.
     * @return
     */
    static Glib::RefPtr<Application> create();

protected:
    /**
     * @brief Create the Application from a non-static context.
     */
    Application();

    void on_startup() override;

    void on_activate() override;

private:
    /**
     * @brief Create the MainWindow and return a mutable, observing pointer.
     * @return A weak non-owning reference to the created MainWindow.
     */
    MainWindow *create_main_window();

    /**
     * @brief Display the 'About' dialog.
     */
    void show_about_dialog() const;

    /**
     * @brief Display the 'Help' web page in the default system browser.
     */
    static void open_application_help();

    /**
     * @brief Quit the application.
     */
    void quit_application();

    Glib::RefPtr<Gtk::Builder> builder;

    Gtk::AboutDialog *about_dialog = nullptr;
};

} // namespace optifol

#endif
