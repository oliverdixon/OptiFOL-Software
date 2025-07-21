/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the external Process Executor
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef TESTEXECUTORBASE_HPP
#define TESTEXECUTORBASE_HPP

#include <glibmm/spawn.h>
#include <gtkmm/textbuffer.h>
#include <string>
#include <vector>

namespace optifol
{

/**
 * @class ProcessExecutor
 * @brief Provides a RAII container for asynchronous execution of external processes under the Glib framework.
 *
 * @details The ProcessExecutor wraps the @ref Glib::spawn_async_with_pipes in an RAII container and provides the
 *  following helpful capabilities:
 *  <ul>
 *      <li>Streaming child <code>stdout</code> to the given @ref Gtk::TextBuffer with a formatted tag;</li>
 *      <li>Streaming child <code>stderr</code> to the given @ref Gtk::TextBuffer with a formatted tag;</li>
 *      <li>Asynchronously executing the child and invoking a synchronous callback on child exit; and</li>
 *      <li>Providing standard RAII destruction capability, such that listening streams are killed correctly.</li>
 *  </ul>
 *
 * @todo Due to buffering, stderr and stdout lines may not appear on the buffer in the order in which they were sent
 *   from the subprocess. This is a known issue and will be fixed.
 */
class ProcessExecutor
{
public:
    /**
     * @brief Construct a new ProcessExecutor container and execute the process according to the given parameters.
     * @param working_directory Working directory in which the process should be started
     * @param argv Argument vector: first element is the executable, followed by any command-line arguments
     * @param envp Key-value list of variables to be exported into the process environment
     * @param output Text buffer in which formatted <code>stdout</code> and <code>stderr</code> entries should be posted
     *  from the child
     * @param finished_callback Callback to invoke once the child exited, taking only the process exit code.
     * @note Even if the ProcessExecutor is destructed, the callback shall still be executed on child exit as the
     *  child-watch signal is managed by the Glib calling context.
     */
    ProcessExecutor(const std::string &working_directory, const std::vector<std::string> &argv,
            const std::vector<std::string> &envp, const Glib::RefPtr<Gtk::TextBuffer> &output,
            sigc::slot<void(int)>&& finished_callback);

private:
    class Stream;

    /**
     * @brief Callback to indicate that new data has arrived on the given stream
     * @param condition The condition under which the data has arrived
     * @param stream_metadata The stream object encapsulating the incoming data
     * @return Was the data received under a non-erroneous condition?
     */
    bool stream_callback(Glib::IOCondition condition, const Stream *stream_metadata) const;

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
            sigc::bound_mem_functor<decltype(&ProcessExecutor::stream_callback), Glib::IOCondition, const Stream *>&&
                write_line_callback);

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

    static const log4cxx::LoggerPtr logger;

    const Glib::RefPtr<Gtk::TextBuffer> output;
    std::optional<Stream> stdout_stream;
    std::optional<Stream> stderr_stream;
    Glib::Pid pid = -1;
};

} // namespace optifol

#endif // TESTEXECUTORBASE_HPP
