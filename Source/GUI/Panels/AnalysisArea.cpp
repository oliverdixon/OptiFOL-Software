/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Analysis and Optimisation view in the Optifol Main Window
 * @author Oliver Dixon
 * @date 2025-04-27
 * @version Development
 */

#include "AnalysisArea.hpp"

#include "../GTKHelpers.hpp"

namespace optifol
{

const char * const AnalysisArea::area_name = "Analysis and Optimisation Area";

AnalysisArea::AnalysisArea(Gtk::Builder &builder) :
    groups_view(GTKHelpers::get_widget<Gtk::ColumnView>(area_name, builder, "analysis_groups_view")),
    context_menu(
        groups_view,
        GTKHelpers::get_object<Gio::Menu>(area_name, builder, "analysis_groups_context_menu"),
        {
            {
                "new_analysis_group",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "new_analysis_group"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "new_analysis_group_popover"),
                true
            },
            {
                "edit_analysis_group",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "edit_analysis_group"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "edit_analysis_group_popover"),
                true
            },
            {
                "delete_analysis_group",
                GTKHelpers::get_widget<Gtk::MenuButton>(area_name, builder, "delete_analysis_group"),
                GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "delete_analysis_group_popover"),
                true
            }
        }
    )
{
}

}
