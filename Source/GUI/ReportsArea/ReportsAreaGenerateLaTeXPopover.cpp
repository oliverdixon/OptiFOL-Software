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

#include "ReportsAreaGenerateLaTeXPopover.hpp"

#include "../Logging.hpp"
#include "../GTKHelpers.hpp"

namespace optifol
{

const char *const ReportsAreaGenerateLaTeXPopover::popover_name = "Generate LaTeX Report Popover";
log4cxx::LoggerPtr ReportsAreaGenerateLaTeXPopover::popover_logger = Logging::get_logger({"ReportingCompliance",
    "GenerateLaTeX"});

ReportsAreaGenerateLaTeXPopover::ReportsAreaGenerateLaTeXPopover(Gtk::Builder &builder) :
    buffer(GTKHelpers::get_widget<Gtk::TextView>(popover_name, builder, "generate_latex_output")->get_buffer()),
    confirm_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "generate_latex_confirm")),
    cancel_button(GTKHelpers::get_widget<Gtk::Button>(popover_name, builder, "generate_latex_cancel")),
    process_stdout(buffer->create_tag("stdout_tag")),
    process_stderr(buffer->create_tag("stderr_tag"))
{
    process_stdout.formatting_tag->property_foreground().set_value("black");
    process_stderr.formatting_tag->property_foreground().set_value("red");

    confirm_button->signal_clicked().connect(sigc::mem_fun(*this,
        &ReportsAreaGenerateLaTeXPopover::confirm_button_callback));
}

ReportsAreaGenerateLaTeXPopover::ConsoleStream::ConsoleStream(
        const Glib::RefPtr<Gtk::TextTag> &formatting_tag) :
    formatting_tag(formatting_tag)
{
}

ReportsAreaGenerateLaTeXPopover::ConsoleStream::~ConsoleStream()
{
    disconnect();
}

void ReportsAreaGenerateLaTeXPopover::ConsoleStream::connect(
        const int source_fd, const sigc::slot<bool(Glib::IOCondition)> &callback_slot)
{
    popover_logger->debug("Latexmk subprocess: attaching to process output buffer with descriptor " +
        std::to_string(source_fd) + '.');

    channel = Glib::IOChannel::create_from_fd(source_fd);
    watch = Glib::signal_io().connect(callback_slot, channel, Glib::IOCondition::IO_IN | Glib::IOCondition::IO_HUP |
        Glib::IOCondition::IO_ERR);
}

void ReportsAreaGenerateLaTeXPopover::ConsoleStream::disconnect()
{
    popover_logger->debug("Latexmk subprocess: disconnecting from subprocess channel");

    channel->close();
    watch.disconnect();
}

bool ReportsAreaGenerateLaTeXPopover::ConsoleStream::is_connected() const noexcept
{
    return watch.connected() || fd != -1;
}
void ReportsAreaGenerateLaTeXPopover::ConsoleStream::append_line_to_buffer(
        const Glib::RefPtr<Gtk::TextBuffer> &target_buffer) const
{
    Glib::ustring line;
    if (channel->read_line(line) == Glib::IOStatus::NORMAL)
        target_buffer->insert_with_tag(target_buffer->end(), line, formatting_tag);
}

void ReportsAreaGenerateLaTeXPopover::confirm_button_callback()
{
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
     *     output directory. This includes the output PDF and all intermediary files (aux, synctex, etc);
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
    Glib::spawn_async_with_pipes(
        "",
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
        &pid,
        nullptr,
        &stdout_fd,
        &stderr_fd
    );

    popover_logger->info("Spawned asynchronous latexmk invocation with PID " + std::to_string(pid) + '.');

    /*
     * Connect to the stdout and stderr streams so we can report verbatim output from latexmk. Just for vanity, any
     * lines from stderr are formatted in red by the Gtk::TextView by way of Gtk::TextTag.
     */
    process_stdout.connect(stdout_fd, sigc::bind(sigc::mem_fun(*this,
        &ReportsAreaGenerateLaTeXPopover::console_stream_callback), &process_stdout));
    process_stderr.connect(stderr_fd, sigc::bind(sigc::mem_fun(*this,
        &ReportsAreaGenerateLaTeXPopover::console_stream_callback), &process_stderr));
}

bool ReportsAreaGenerateLaTeXPopover::console_stream_callback(
        const Glib::IOCondition condition, ConsoleStream *stream_metadata) const
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
