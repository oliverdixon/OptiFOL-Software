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

#include <ranges>

#include "../DocumentGeneration/LaTeXGeneratorManager.hpp"
#include "../GTKHelpers.hpp"

namespace optifol
{

const char * const ReportsArea::area_name = "Reporting and Compliance Area";
const std::string ReportsArea::LatexmkCommand::base_command = "latexmk -pdf -interaction=nonstopmode";
const std::string ReportsArea::LatexmkCommand::template_path = "Resources/ReportTemplates/LaTeX/report.tex";

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
    )
{
    configure_generate_latex_popover(builder);
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

void ReportsArea::configure_generate_latex_popover(Gtk::Builder &builder)
{
    const auto popover = GTKHelpers::get_widget<Gtk::Popover>(area_name, builder, "reports_generate_latex_popover");
    const auto confirm_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "generate_latex_confirm");
    const auto cancel_button = GTKHelpers::get_widget<Gtk::Button>(area_name, builder, "generate_latex_cancel");
    const auto command = GTKHelpers::get_widget<Gtk::Entry>(area_name, builder, "generate_latex_command");
    const auto output_area_buffer = GTKHelpers::get_widget<Gtk::TextView>(area_name, builder,
        "generate_latex_output")->get_buffer();

    const auto buffer_stdout_tag = output_area_buffer->create_tag("stdout_tag");
    const auto buffer_stderr_tag = output_area_buffer->create_tag("stderr_tag");
    buffer_stdout_tag->property_foreground().set_value("black");
    buffer_stderr_tag->property_foreground().set_value("red");

    cancel_button->signal_clicked().connect([popover]
    {
        popover->popdown();
    });

    // TODO move this into a separate member function
    // Or, just have a separate class for this popover and incorporate LaTeXGeneratorManager...
    confirm_button->signal_clicked().connect([this, popover, output_area_buffer, command]
    {
        output_area_buffer->erase(output_area_buffer->begin(), output_area_buffer->end());
        command->get_buffer()->set_text(optifol::ReportsArea::LatexmkCommand::base_command +
            latexmk_command.output_path + optifol::ReportsArea::LatexmkCommand::template_path);

        Glib::Pid pid;
        int stdout_fd;
        int stderr_fd;

        Glib::spawn_async_with_pipes(
            "", // Current working directory
            {
                "latexmk",
                "-pdf",
                "-interaction=nonstopmode",
                "-outdir=GeneratedReports/test/",
                "Resources/ReportTemplates/LaTeX/report.tex"
            },
            {
                "PATH=" + Glib::getenv("PATH"),
                "TEXINPUTS=.:GeneratedReports/test:"
            },
            Glib::SpawnFlags::SEARCH_PATH,
            {},
            &pid, // TODO log4cxx message with this? Not used for anything else.
            nullptr,
            &stdout_fd,
            &stderr_fd
        );

        const auto& tag_table = output_area_buffer->get_tag_table();

        // TODO how do we manage these? When are they destructed?
        latexmk_stdout_manager = std::make_unique<LaTeXGeneratorManager>(output_area_buffer, stdout_fd,
            tag_table->lookup("stdout_tag"));
        latexmk_stderr_manager = std::make_unique<LaTeXGeneratorManager>(output_area_buffer, stderr_fd,
            tag_table->lookup("stderr_tag"));
    });
}

} // namespace optifol
