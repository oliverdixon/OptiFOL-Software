/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Reports Area "Generate LaTeX" popover manager
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#include <fstream>

#include "ReportsAreaGenerateLaTeXPopover.hpp"
#include "../GTKHelpers.hpp"
#include "../Logging.hpp"
#include "ReportsArea.hpp"

namespace optifol
{

const char * const ReportsAreaGenerateLaTeXPopover::popover_name = "Generate LaTeX Report Popover";
const log4cxx::LoggerPtr ReportsAreaGenerateLaTeXPopover::popover_logger =
        Logging::get_logger({"GUI", "ReleasesReports", "GenerateLaTeX"});

ReportsAreaGenerateLaTeXPopover::ReportsAreaGenerateLaTeXPopover(
        Gtk::Builder &builder, const ReportsArea& reports_area) :
    reports_area(reports_area),
    buffer(GTKHelpers::get_widget<Gtk::TextView>(popover_name, builder, "generate_latex_output")->get_buffer()),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "reports_generate_latex_popover")),
    details_container(GTKHelpers::get_widget<Gtk::Box>(popover_name, builder, "generate_latex_details_container")),
    output_directory_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "generate_latex_output_path")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "generate_latex_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "generate_latex_cancel")),
    open_dialog(GTKHelpers::get_object<Gtk::FileDialog>(
            popover_name, builder, "generate_latex_output_path_chooser_dialog")),
    show_details_check(GTKHelpers::get_widget<Gtk::CheckButton>(popover_name, builder, "generate_latex_show_details"))
{

    show_details_check->signal_toggled().connect(
            sigc::mem_fun(*this, &ReportsAreaGenerateLaTeXPopover::show_details_toggled));
    confirm_button->signal_clicked().connect(
            sigc::mem_fun(*this, &ReportsAreaGenerateLaTeXPopover::confirm_button_clicked));

    const auto open_directory_button =
            GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "generate_latex_output_path_chooser_button");
    open_directory_button->signal_clicked().connect(
            sigc::mem_fun(*this, &ReportsAreaGenerateLaTeXPopover::open_directory_button_clicked));
}

void ReportsAreaGenerateLaTeXPopover::confirm_button_clicked()
{
    confirm_button->set_sensitive(false);
    buffer->set_text("");

    generator.emplace(output_directory);

    reports_area.observe_active_subsystem()->for_each(
        [this](const Requirement &requirement)
        {
            generator->add_requirement(requirement);
        });

    const auto test_groups = reports_area.observe_active_subsystem()->get_test_groups();
    const auto test_group_count = test_groups->get_n_items();
    for (guint test_group_index = 0; test_group_index < test_group_count; ++test_group_index)
        generator->add_test_group(*test_groups->get_item(test_group_index));

    generator->generate(buffer,
            [this]
            {
                generator.reset();
                confirm_button->set_sensitive();
            });
}

void ReportsAreaGenerateLaTeXPopover::cancel_button_clicked()
{
    my_popover->popdown();
    clear_inputs();
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive: called from button-click callback.
void ReportsAreaGenerateLaTeXPopover::clear_inputs()
{
    output_directory_entry->set_text("");
    buffer->set_text("");
    output_directory = nullptr;
}

void ReportsAreaGenerateLaTeXPopover::open_directory_button_clicked()
{
    // Hide the popover whilst the dialog is active, otherwise it may have z-index priority over the dialog.
    my_popover->popdown();
    open_dialog->select_folder(sigc::mem_fun(*this, &ReportsAreaGenerateLaTeXPopover::open_directory_finished));
}

void ReportsAreaGenerateLaTeXPopover::open_directory_finished(const Glib::RefPtr<Gio::AsyncResult> &result)
{
    try {
        output_directory = open_dialog->select_folder_finish(result);
    } catch (const Gtk::DialogError &dialog_error) {
        popover_logger->info("Directory selection dialog closed without feedback; state unchanged: " +
                std::string(dialog_error.what()));
    } catch (const Glib::Error &library_error) {
        popover_logger->error("Unexpected error from directory selection dialog; state unchanged: " +
                std::string(library_error.what()));
    }

    // Restore the temporarily hidden popover
    my_popover->popup();

    // Always update the read-only text entry with the up-to-date output directory path
    output_directory_entry->set_text(output_directory == nullptr ? "" : output_directory->get_path());
}

void ReportsAreaGenerateLaTeXPopover::show_details_toggled() const
{
    details_container->set_visible(show_details_check->get_active());
}

} // namespace optifol
