//
// Created by owd on 18/01/25.
//

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

    void on_activate() override;

private:
    MainWindow* create_main_window();
};

}

#endif //OPTIFOL_APPLICATION_HPP
