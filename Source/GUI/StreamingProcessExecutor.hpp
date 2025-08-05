/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the external streaming Process Executor
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#ifndef STREAMINGPROCESSEXECUTOR_HPP
#define STREAMINGPROCESSEXECUTOR_HPP

#include <gtkmm/textbuffer.h>
#include <glibmm/iochannel.h>

#include "ProcessExecutor.hpp"

namespace optifol
{

/**
 * @class StreamingProcessExecutor
 * @brief Provides a ProcessExecutor with the capability to capture <code>stdout</code> and <code>stderr</code> streams
 *  from the sub-process and write the colour-formatted lines to a Gtk::TextBuffer.
 * @see ProcessExecutor for semantics of external sub-process management.
 */
class StreamingProcessExecutor :
        public ProcessExecutor
{
public:
    /**
     * @brief Asynchronously execute an external process at the given working directory, with the given argument vector,
     *  optionally including a key-value environment specification and exit callback.
     * @param working_directory The working directory in which the sub-process should be started.
     * @param argv The argument vector, where the first entry is the command to execute. The system PATH will be
     *  searched to find the executable, but it is not run under a shell.
     * @param envp The optional <code>KEY=VALUE</code> set of environment variables to export into the sub-process
     *  environment.
     * @param output The destination output buffer to which the sub-process <code>stdout</code> and <code>stderr</code>
     *  lines should be redirected.
     * @param finished_callback The noexcept callback to execute on completion of the sub-process, taking the numerical
     *  exit code.
     */
    StreamingProcessExecutor(const std::string &working_directory, const std::vector<std::string> &argv,
            const std::vector<std::string> &envp, const Glib::RefPtr<Gtk::TextBuffer> &output,
            sigc::slot<void(int)>&& finished_callback);

    /**
     * @brief Statically write a Glib::SpawnError exception message, formatted as an error, to the given output.
     * @param exception The exception to describe on the output buffer.
     * @param output The Gtk::TextBuffer to receive the human-readable exception message.
     */
    static void write_exception_error(const Glib::SpawnError &exception, const Glib::RefPtr<Gtk::TextBuffer> &output);

private:
    static const log4cxx::LoggerPtr logger;

    class Stream;

    /**
     * @brief Callback to indicate that new data has arrived at the given stream.
     * @param condition The condition under which the data has arrived.
     * @param stream_metadata The stream object encapsulating the incoming data.
     * @pre The @ref output buffer is not null.
     * @return Was the data received under a non-erroneous condition?
     */
    bool stream_callback(Glib::IOCondition condition, const Stream *stream_metadata) const;

    /**
     * @copybrief ProcessExecutor::reap_child
     * @details In addition to reaping the child process and calling the user-supplied callback, the streams are first
     *  disconnected to prevent thread hangs.
     * @param ended_pid The PID of the ended process.
     * @param exit_code The numerical exit code of the process; anything except zero is deemed indicative of an error.
     */
    void reap_child(Glib::Pid ended_pid, int exit_code) noexcept override;

    /**
     * @brief Get or create a formatted colour tag on the given output buffer.
     * @param output The output to which format-tagged lines should be written.
     * @param name The internal name of the GTK tag.
     * @param colour_name The name of the colour to use, e.g. "red".
     * @return The formatting tag.
     */
    static Glib::RefPtr<Gtk::TextTag> get_colour_tag(
            const Glib::RefPtr<Gtk::TextBuffer> &output, const Glib::ustring &name, const Glib::ustring &colour_name);

    /**
     * @class Stream
     * @brief Helper for watching a single stream and despatching a callback on new lines of data.
     */
    class Stream
    {
    public:
        /**
         * @brief Construct a new Stream to watch the data on the given file descriptor
         * @param formatting_tag The @ref Gtk::TextTag containing formatting information for data produced by the Stream
         * @param source_fd The numerical file descriptor of the Stream to watch
         * @param write_line_callback The callback to execute when new data becomes available on the Stream
         */
        explicit Stream(const Glib::RefPtr<Gtk::TextTag> &formatting_tag, int source_fd,
            sigc::bound_mem_functor<
                decltype(&StreamingProcessExecutor::stream_callback),
                Glib::IOCondition,
                const Stream *
            >&& write_line_callback);

        /**
         * @brief Destruct the Stream by flushing the buffers, invoking any callbacks, and disconnecting watches and
         *  signals.
         */
        ~Stream();

        /**
         * @brief Append the newest line of data on the Stream to the given buffer using the fixed formatting tag
         * @param target_buffer The target buffer to receive the latest Stream line of data
         */
        void append_line_to_buffer(const Glib::RefPtr<Gtk::TextBuffer> &target_buffer) const;

    private:
        Glib::RefPtr<Gtk::TextTag> formatting_tag;
        const Glib::RefPtr<Glib::IOChannel> channel;
        sigc::slot<bool(Glib::IOCondition)> bound_write_line_callback;
        sigc::connection watch;
    };

    const Glib::RefPtr<Gtk::TextBuffer> output;
    std::optional<Stream> stdout_stream;
    std::optional<Stream> stderr_stream;
};

} // namespace optifol

#endif // STREAMINGPROCESSEXECUTOR_HPP
