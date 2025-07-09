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

const char *const ReportsAreaGenerateLaTeXPopover::popover_name = "Generate LaTeX Report Popover";
const log4cxx::LoggerPtr ReportsAreaGenerateLaTeXPopover::popover_logger =
        Logging::get_logger({"ReportingCompliance", "GenerateLaTeX"});

ReportsAreaGenerateLaTeXPopover::ReportsAreaGenerateLaTeXPopover(
        Gtk::Builder &builder, const ReportsArea *reports_area) :
    reports_area(reports_area),
    buffer(GTKHelpers::get_widget<Gtk::TextView>(popover_name, builder, "generate_latex_output")->get_buffer()),
    my_popover(GTKHelpers::get_widget<Gtk::Popover>(popover_name, builder, "reports_generate_latex_popover")),
    details_container(GTKHelpers::get_widget<Gtk::Box>(popover_name, builder, "generate_latex_details_container")),
    output_directory_entry(GTKHelpers::get_widget<Gtk::Entry>(popover_name, builder, "generate_latex_output_path")),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "generate_latex_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "generate_latex_cancel")),
    open_dialog(GTKHelpers::get_object<Gtk::FileDialog>(
            popover_name, builder, "generate_latex_output_path_chooser_dialog")),
    show_details_check(GTKHelpers::get_widget<Gtk::CheckButton>(popover_name, builder, "generate_latex_show_details")),
    process_stdout(buffer->create_tag("stdout_tag")),
    process_stderr(buffer->create_tag("stderr_tag"))
{
    process_stdout.formatting_tag->property_foreground().set_value("black");
    process_stderr.formatting_tag->property_foreground().set_value("red");

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
    // TODO: disable confirm button during compilation. Not sure how?

    update_requirements_csv();
    buffer->erase(buffer->begin(), buffer->end());

    Glib::Pid pid;
    int stdout_fd;
    int stderr_fd;

    /*
     * Use Glib helper to spawn the process with the following attributes:
     *
     *   - Do not change process working directory, as all further paths are assumed relative to Optifol's CWD;
     *
     *   - Run latexmk in non-interactive mode to generate a PDF from the Resources TeX template in the user-specified
     *     output directory. This includes the output PDF and all intermediary files (aux, fls, etc);
     *
     *   - Inherit the PATH from Optifol's environment, such that the latexmk and pdflatex executables can be located on
     *     the system, and append the TEXINPUTS to include the generated CSV containing subsystem information.
     *
     *   - Use the SpawnFlags::SEARCH_PATH execution flag to search the path for the executable. There's a slight
     *     overhead here, but it allows us to (a) make use of the system path functionality for latexmk and all
     *     subprocesses, such as pdflatex; (b) not have to specify an absolute path; (c) not have to wrap everything in
     *     a heavy shell like Bash;
     *
     *   - No pre-exec() callback required;
     *
     *   - Capture PID for logging purposes; do not capture stdin FD; capture stdout FD; capture stderr FD.
     *
     *  It remains to be seen how/if platform-independent this approach is. It's definitely better than using POSIX or
     *  system calls.
     */
    // clang-format off
    Glib::spawn_async_with_pipes("",
        {
            "latexmk",
            "-pdf",
            "-interaction=nonstopmode",
            "-outdir=" + output_directory->get_path(),
            "Resources/ReportTemplates/LaTeX/report.tex"
        },
        {
            "PATH=" + Glib::getenv("PATH"),
            "TEXINPUTS=.:" + output_directory->get_path() + ":"
        },
        Glib::SpawnFlags::SEARCH_PATH,
        {},
        &pid,
        nullptr,
        &stdout_fd,
        &stderr_fd
    );
    // clang-format on

    popover_logger->info("Spawned asynchronous latexmk invocation with PID " + std::to_string(pid) + '.');

    /*
     * Connect to the stdout and stderr streams so we can report verbatim output from latexmk. Just for vanity, any
     * lines from stderr are formatted in red by the Gtk::TextView by way of Gtk::TextTag.
     */
    // clang-format off
    process_stdout.connect(stdout_fd, sigc::bind(
                sigc::mem_fun(*this, &ReportsAreaGenerateLaTeXPopover::console_stream_callback), &process_stdout));
    process_stderr.connect(stderr_fd, sigc::bind(
                sigc::mem_fun(*this, &ReportsAreaGenerateLaTeXPopover::console_stream_callback), &process_stderr));
    // clang-format on
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

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive; function called by button callback.
void ReportsAreaGenerateLaTeXPopover::update_requirements_csv() const
{
    const auto csv_path = output_directory->get_path() + "/index.csv";
    popover_logger->info("Writing CSV requirements index for consumption by LaTeX template at " + csv_path + '.');

    std::ofstream file_stream(csv_path);
    const auto observed_requirements = reports_area->observe_active_subsystem()->requirements.get();
    const auto requirement_count = observed_requirements->get_n_items();

    for (guint requirement_idx = 0; requirement_idx < requirement_count; ++requirement_idx) {
        const auto &requirement = observed_requirements->get_item(requirement_idx);
        file_stream << requirement->property_name().get_value() << ','
                    << requirement->property_description().get_value() << ',' << '$'
                    << requirement->observe_latex_statement() << '$' << '\n';
    }

    file_stream.flush();
}

void ReportsAreaGenerateLaTeXPopover::show_details_toggled() const
{
    details_container->set_visible(show_details_check->get_active());
}
bool ReportsAreaGenerateLaTeXPopover::console_stream_callback(
        Glib::IOCondition condition, struct ProcessStream *stream_metadata) const
{
    if ((condition & Glib::IOCondition::IO_IN) == Glib::IOCondition::IO_IN) {
        stream_metadata->append_line_to_buffer(buffer);
        return true;
    }

    /*
     * If we've triggered the callback with an IOCondition::IO_IN, something unexpected has happened. Bail out and
     * disconnect so we don't risk a dangling FD reference.
     */
    stream_metadata->disconnect();
    return false;
}

} // namespace optifol
