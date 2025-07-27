/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the external streaming Process Executor
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#include <cassert>
#include <glibmm/main.h>

#include "../Logging.hpp"
#include "StreamingProcessExecutor.hpp"

namespace optifol
{

const log4cxx::LoggerPtr StreamingProcessExecutor::logger = Logging::get_logger({"SubprocessControl",
    "StreamingProcessExecutor"});

StreamingProcessExecutor::StreamingProcessExecutor(const std::string &working_directory,
        const std::vector<std::string> &argv, const std::vector<std::string> &envp,
        const Glib::RefPtr<Gtk::TextBuffer> &output, sigc::slot<void(int)> &&finished_callback) :
    ProcessExecutor(working_directory, argv, envp, std::move(finished_callback)),
    output(output)
{
    stdout_stream.emplace(get_colour_tag(output, "stdout_tag", "black"), stdout_fd,
        sigc::mem_fun(*this, &StreamingProcessExecutor::stream_callback));

    stderr_stream.emplace(get_colour_tag(output, "stderr_tag", "red"), stderr_fd,
        sigc::mem_fun(*this, &StreamingProcessExecutor::stream_callback));

    logger->info("Spawned tracking sub-process \"" + argv[0] + "\" with PID " + std::to_string(pid) + '.');
}

void StreamingProcessExecutor::write_exception_error(
        const Glib::SpawnError &exception, const Glib::RefPtr<Gtk::TextBuffer> &output)
{
    output->insert_with_tag(output->end(), exception.what(), get_colour_tag(output, "stderr_tag", "red"));
}

bool StreamingProcessExecutor::stream_callback(
        const Glib::IOCondition condition, const Stream *const stream_metadata) const
{
    assert(output != nullptr);

    if (std::to_underlying(condition & (Glib::IOCondition::IO_IN | Glib::IOCondition::IO_HUP)) != 0) {
        stream_metadata->append_line_to_buffer(output);
        return true;
    }

    /*
     * If we've triggered the callback with something other than a IOCondition::IO_IN or IOCondition::IO_HUP, something
     * unexpected has happened and Glib is indicating an error state.
     */
    logger->warn("Abnormal IO condition reported by GLib for sub-process stream: code " +
            std::to_string(std::to_underlying(condition)) + '.');
    return false;
}

void StreamingProcessExecutor::reap_child(const Glib::Pid ended_pid, const int exit_code) noexcept
{
    if (ended_pid != pid)
        return;

    /*
     * We would like to keep to RAII as much as practicable, but the streams must be irrevocably reset upon sub-process
     * termination otherwise the entire thread will hang.
     */
    stdout_stream.reset();
    stderr_stream.reset();

    // Call this last, as it invokes a synchronous user-provided callback that may require disconnected streams.
    ProcessExecutor::reap_child(ended_pid, exit_code);
}

Glib::RefPtr<Gtk::TextTag> StreamingProcessExecutor::get_colour_tag(
        const Glib::RefPtr<Gtk::TextBuffer> &output, const Glib::ustring &name, const Glib::ustring &colour_name)
{
    auto tag(output->get_tag_table()->lookup(name));

    if (tag == nullptr) {
        tag = output->create_tag(name);
        tag->property_foreground().set_value(colour_name);
    }

    return tag;
}

StreamingProcessExecutor::Stream::Stream(const Glib::RefPtr<Gtk::TextTag> &formatting_tag, int source_fd,
        sigc::bound_mem_functor<decltype(&StreamingProcessExecutor::stream_callback), Glib::IOCondition, const Stream *>
                &&write_line_callback) :
    formatting_tag(formatting_tag),
    channel(Glib::IOChannel::create_from_fd(source_fd)),
    bound_write_line_callback(sigc::bind(write_line_callback, this)),
    watch(Glib::signal_io().connect(
        bound_write_line_callback,
        channel,
        Glib::IOCondition::IO_IN | Glib::IOCondition::IO_HUP | Glib::IOCondition::IO_ERR
    ))
{
}

StreamingProcessExecutor::Stream::~Stream()
{
    try {
        /*
         * Even with the buffers flushed, we manually execute the callback just to be safe. In the worst case, there's
         * nothing to read. We explicitly ignore the return code of the callback, as during destruction we don't care if
         * the channel is reporting a HUP.
         */
        if (channel->flush() == Glib::IOStatus::NORMAL)
            std::ignore = bound_write_line_callback(Glib::IOCondition::IO_IN);

        watch.disconnect();
        channel->close();
    } catch (const Glib::IOChannelError &channel_error) {
        logger->error("Could not graciously destruct stream for sub-process.");
        logger->error(channel_error.what());
    }
}

void StreamingProcessExecutor::Stream::append_line_to_buffer(const Glib::RefPtr<Gtk::TextBuffer> &target_buffer) const
{
    Glib::ustring content;

    // Read to the end to receive all content since the last read. There is no need to delimit on line boundaries.
    if (channel->read_to_end(content) == Glib::IOStatus::NORMAL)
        target_buffer->insert_with_tag(target_buffer->end(), content, formatting_tag);
}

} // namespace optifol
