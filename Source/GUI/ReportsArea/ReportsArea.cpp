/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Main Window's Reporting and Compliance area
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#include "ReportsArea.hpp"
#include "../GTKHelpers.hpp"

namespace optifol
{

const char * const ReportsArea::area_name = "Reporting and Compliance Area";

ReportsArea::ReportsArea(Gtk::Builder &builder) :
    on_off_widgets(
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "reports_advice_unselected"),
        GTKHelpers::get_widget<Gtk::Widget>(area_name, builder, "reports_content")
    ),
    view(GTKHelpers::get_widget<Gtk::ColumnView>(area_name, builder, "reports_elements_view")),
    context_menu(
        view,
        GTKHelpers::get_object<Gio::Menu>(area_name, builder, "reports_context_menu"),
        {
            {
                "reports_generate_latex",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "reports_generate_latex"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "reports_generate_latex_popover"),
                true
            }
        }
    ),
    generate_latex_popover(builder)
{
}

void ReportsArea::select_model(const Glib::RefPtr<const Subsystem> &subsystem)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);
}

void ReportsArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);
}

} // namespace optifol
