/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Analysis Area "New Analysis Group" popover manager
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#include "AnalysisAreaNewAnalysisGroupPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "../Storage/AnalysisGroup.hpp"
#include "../Storage/Subsystem.hpp"
#include "AnalysisArea.hpp"

namespace optifol
{

const char *const AnalysisAreaNewAnalysisGroupPopover::popover_name = "New Analysis Group Popover";
const log4cxx::LoggerPtr AnalysisAreaNewAnalysisGroupPopover::popover_logger =
        Logging::get_logger({"GUI", "AnalysisOptimisation", "NewAnalysisGroup"});

AnalysisAreaNewAnalysisGroupPopover::AnalysisAreaNewAnalysisGroupPopover(
        Gtk::Builder &builder, AnalysisArea &analysis_area) :
    analysis_area(analysis_area),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "new_analysis_group_popover")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "new_analysis_group_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "new_analysis_group_cancel")),
    name_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "new_analysis_group_property_name"))
{
    confirm_button->signal_clicked().connect(
            sigc::mem_fun(*this, &AnalysisAreaNewAnalysisGroupPopover::confirm_button_clicked));
    cancel_button->signal_clicked().connect(
            sigc::mem_fun(*this, &AnalysisAreaNewAnalysisGroupPopover::cancel_button_clicked));
}

void AnalysisAreaNewAnalysisGroupPopover::confirm_button_clicked() const
{
    my_popover->popdown();
    analysis_area.observe_active_subsystem()->get_analysis_groups()->append(
            Glib::make_refptr_for_instance(new AnalysisGroup(name_entry->get_text(),
                    analysis_area.get_active_subsystem()->share_symbol_repository())));

    popover_logger->debug("Created new analysis group with name \"" + name_entry->get_text() + "\".");
    clear_inputs();
}

void AnalysisAreaNewAnalysisGroupPopover::cancel_button_clicked() const
{
    my_popover->popdown();
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void AnalysisAreaNewAnalysisGroupPopover::clear_inputs() const
{
    name_entry->set_text("");
}

} // namespace optifol
