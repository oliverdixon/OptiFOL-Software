/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Reports Area "Generate LaTeX" popover manager
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#ifndef REPORTSAREAGENERATELATEXPOPOVER_HPP
#define REPORTSAREAGENERATELATEXPOPOVER_HPP

#include <glibmm/iochannel.h>
#include <glibmm/refptr.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/textbuffer.h>
#include <log4cxx/logger.h>

namespace optifol
{

/**
 * @class ReportsAreaGenerateLaTeXPopover
 * @brief Manage the "Generate LaTeX" popover for the Reporting and Compliance Area
 * @note Instantiations of this class mutate the graphical environment via the given Gtk::Builder. As such, it should be
 *  regarded an effective singleton as multiple instantiations will cause conflicts with the GTK+ runtime due to
 *  multiply registered callbacks.
 * @see ReportsArea for parent area
 * @details
 *  <p>
 *      The "Generate LaTeX" popover provides controls for setting LaTeX-specific options in the automated report-
 *      generation process and invoking the external commands to construct a LaTeX and subsequent PDF document. The
 *      following GTK elements are expected from the given Gtk::Builder:
 *      <table>
 *          <tr>
 *              <th>GTK C++ Class</th>
 *              <th>Unique Identifier</th>
 *          </tr>
 *          <tr>
 *              <td>Gtk::TextView</td>
 *              <td><code>generate_latex_output</code></td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Button</td>
 *              <td><code>generate_latex_confirm</code></td>
 *          </tr>
 *          <tr>
 *              <td>Gtk::Button</td>
 *              <td><code>generate_latex_cancel</code></td>
 *          </tr>
 *      </table>
 *      A @ref std::runtime_error will be thrown by ReportsAreaGenerateLaTeXPopover(Gtk::Builder&) if any of these are
 *      inaccessible in the expected type instantiations.
 *  </p>
 *  <p>
 *      The implementation and state machine of this class is complicated by the process of despatching and watching the
 *      output buffers (via numerical file descriptors for <code>stdout</code> and <code>stderr</code>) of external
 *      processes; in particular, the <code>latexmk</code> utility used to build PDFs from LaTeX (<code>.tex</code>)
 *      source files. The implementation here is believed to be reasonably fool-proof, albeit specialised to the single
 *      use-case, and elegantly handles data from both standard output streams, errors, and HUPs indicating process
 *      completion.
 *  </p>
 */
class ReportsAreaGenerateLaTeXPopover
{
public:
    /**
     * @brief Construct a new popover manager, registering callbacks on elements loaded by the given builder
     * @param builder A GTK builder containing popover UI elements
     * @throws std::runtime_error A required GTK element/widget could not be loaded from the given builder
     */
    explicit ReportsAreaGenerateLaTeXPopover(Gtk::Builder& builder);

private:
    /**
     * @class ConsoleStream
     * @brief RAII wrapper for a connection to the streaming data buffer (nominally a pipe or socket) of an external
     *  process, with support for writing formatted data to a Gtk::TextBuffer.
     */
    struct ConsoleStream
    {
        /**
         * @brief Create a new empty ConsoleStream
         * @param formatting_tag The constant formatting tag to use for line formatting of the stream data in
         *  Gtk::TextView windows.
         */
        explicit ConsoleStream(const Glib::RefPtr<Gtk::TextTag>& formatting_tag);

        /**
         * @brief RAII-destruct the ConsoleStream by disconnecting any system/subprocess references.
         */
        ~ConsoleStream();

        /**
         * @brief Prevent copying as it causes unnecessary complexity and is not semantically necessary.
         */
        ConsoleStream(const ConsoleStream&) = delete;

        /**
         * @brief Prevent moving as it causes unnecessary complexity and is not semantically necessary.
         * @see @ref disconnect()
         */
        ConsoleStream(ConsoleStream&&) = delete;

        /**
         * @brief Connect a new FD to the ConsoleStream slot
         * @param source_fd The numerical file descriptor of the stream to watch
         * @param callback_slot The callback for events on the stream identified by the given descriptor
         */
        void connect(int source_fd, const sigc::slot<bool(Glib::IOCondition)> &callback_slot);

        /**
         * @brief Disconnect the stream, releasing all held resources except the formatting tag shared reference.
         */
        void disconnect();

        /**
         * @brief Query the connection state of the ConsoleStream slot
         * @return Does the ConsoleStream hold an active connection?
         */
        [[nodiscard]] bool is_connected() const noexcept;

        /**
         * @brief Write the latest line from the stream to the end of the given Gtk::TextBuffer with the stream's
         *  formatting tag
         * @param target_buffer The target buffer to which data should be appended
         */
        void append_line_to_buffer(const Glib::RefPtr<Gtk::TextBuffer>& target_buffer) const;

        const Glib::RefPtr<Gtk::TextTag> formatting_tag;

    private:
        static log4cxx::LoggerPtr logger;

        sigc::connection watch;
        Glib::RefPtr<Glib::IOChannel> channel;
        int fd = -1;
    };

    /**
     * @brief Handle a press of the "Confirm" button by generating LaTeX with latexmk and providing real-time feedback
     */
    void confirm_button_callback();

    /**
     * @brief Handle new data appearing on a subprocess output stream by appending to the popover output
     * @param condition The condition(s) responsible for invoking the callback
     * @param stream_metadata The ConsoleStream slot of the origin subprocess stream
     * @return <code>true</code> if data was successfully copied to the console output; <code>false</code> otherwise.
     */
    bool console_stream_callback(Glib::IOCondition condition, ConsoleStream *stream_metadata) const;

    static const char * const popover_name;
    static log4cxx::LoggerPtr popover_logger;

    const Glib::RefPtr<Gtk::TextBuffer> buffer;
    Gtk::Button * const confirm_button;
    Gtk::Button * const cancel_button;

    ConsoleStream process_stdout;
    ConsoleStream process_stderr;
};

} // namespace optifol

#endif // REPORTSAREAGENERATELATEXPOPOVER_HPP
