/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Main Window's Releases and Reports area
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#include "ReportsArea.hpp"
#include "../GTKHelpers.hpp"

namespace optifol
{

const char * const ReportsArea::area_name = "Releases and Reports Area";

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
    generate_latex_popover(builder, *this)
{
}

void ReportsArea::select_model(const Glib::RefPtr<Subsystem> &subsystem)
{
    on_off_widgets.first->set_visible(false);
    on_off_widgets.second->set_visible(true);
    active_subsystem = subsystem;
}

void ReportsArea::deselect_model()
{
    on_off_widgets.second->set_visible(false);
    on_off_widgets.first->set_visible(true);
    active_subsystem = nullptr;
}

Subsystem *ReportsArea::observe_active_subsystem() noexcept
{
    return active_subsystem.get();
}

const Subsystem *ReportsArea::observe_active_subsystem() const noexcept
{
    return active_subsystem.get();
}

} // namespace optifol
