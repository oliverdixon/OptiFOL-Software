//
// Created by owd on 18/01/25.
//

#include "MainWindow.hpp"

namespace optifol
{

MainWindow::MainWindow()
{
    set_title("OptiFOL");
    set_default_size(600, 400);

    builder = Gtk::Builder::create_from_resource("/uk/ac/york/www_users/od641/optifol/UI/MainWindow.ui");
    dialog = builder->get_widget<Gtk::AboutDialog>("about_dialog");

    if (!dialog)
        throw std::runtime_error("Missing about_dialog widget in MainWindow.ui");

    dialog->set_transient_for(*this);
    dialog->set_hide_on_close();

    dialog->set_visible(true);
    dialog->present();
}

}
