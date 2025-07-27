/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the external Process Executor
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#include <glibmm/main.h>

#include "../Logging.hpp"
#include "ProcessExecutor.hpp"

#include <cassert>
#include <iostream>

namespace optifol
{

const log4cxx::LoggerPtr ProcessExecutor::logger = Logging::get_logger({"SubprocessControl", "ProcessExecutor"});

ProcessExecutor::ProcessExecutor(const std::string &working_directory, const std::vector<std::string> &argv,
        const std::vector<std::string> &envp, const Glib::RefPtr<Gtk::TextBuffer> &output,
        sigc::slot<void(int)> &&finished_callback) :
    output(output)
{
    const auto tag_table = output->get_tag_table();
    auto stdout_tag(tag_table->lookup("stdout_tag"));
    auto stderr_tag(tag_table->lookup("stderr_tag"));

    if (stdout_tag == nullptr) {
        stdout_tag = output->create_tag("stdout_tag");
        stdout_tag->property_foreground().set_value("black");
    }

    if (stderr_tag == nullptr) {
        stderr_tag = output->create_tag("stderr_tag");
        stderr_tag->property_foreground().set_value("red");
    }

    try {
        auto [stdout_fd, stderr_fd] = spawn_process(working_directory, argv, envp, std::move(finished_callback));

        stdout_stream.emplace(stdout_tag, stdout_fd, sigc::mem_fun(*this, &ProcessExecutor::stream_callback));
        stderr_stream.emplace(stderr_tag, stderr_fd, sigc::mem_fun(*this, &ProcessExecutor::stream_callback));

        logger->info("Spawned tracking sub-process \"" + argv[0] + "\" with PID " + std::to_string(pid) + '.');
    } catch (const Glib::SpawnError& spawn_error) {
        output->insert_with_tag(output->end(), spawn_error.what(), stderr_tag);
        throw;
    }
}

ProcessExecutor::ProcessExecutor(const std::string &working_directory, const std::vector<std::string> &argv,
        const std::vector<std::string> &envp, sigc::slot<void(int)> &&finished_callback)
{
    std::ignore = spawn_process(working_directory, argv, envp, std::move(finished_callback));
}

ProcessExecutor::Stream::Stream(const Glib::RefPtr<Gtk::TextTag> &formatting_tag, const int source_fd,
        sigc::bound_mem_functor<decltype(&ProcessExecutor::stream_callback), Glib::IOCondition, const Stream *>
                &&write_line_callback) :
    formatting_tag(formatting_tag),
    channel(Glib::IOChannel::create_from_fd(source_fd)),
    bound_write_line_callback(sigc::bind(write_line_callback, this)),
    watch(Glib::signal_io().connect(bound_write_line_callback, channel,
            Glib::IOCondition::IO_IN | Glib::IOCondition::IO_HUP | Glib::IOCondition::IO_ERR))
{
}

ProcessExecutor::Stream::~Stream()
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

void ProcessExecutor::Stream::append_line_to_buffer(const Glib::RefPtr<Gtk::TextBuffer> &target_buffer) const
{
    Glib::ustring line;
    if (channel->read_to_end(line) == Glib::IOStatus::NORMAL)
        target_buffer->insert_with_tag(target_buffer->end(), line, formatting_tag);
}

bool ProcessExecutor::stream_callback(const Glib::IOCondition condition, const Stream *stream_metadata) const
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

std::pair<int, int> ProcessExecutor::spawn_process(const std::string& working_directory,
        const std::vector<std::string> &argv, const std::vector<std::string> &envp,
        sigc::slot<void(int)> &&finished_callback)
{
    int stdout_fd;
    int stderr_fd;

    try {
        Glib::spawn_async_with_pipes(working_directory, argv, envp,
                Glib::SpawnFlags::SEARCH_PATH | Glib::SpawnFlags::DO_NOT_REAP_CHILD, {}, &pid, nullptr, &stdout_fd,
                &stderr_fd);
    } catch (const Glib::SpawnError &spawn_error) {
        logger->error("Failed to spawn sub-process.");
        logger->error(spawn_error.what());
        throw;
    }

    Glib::signal_child_watch().connect(
        [this, finished_callback](const Glib::Pid ended_pid, const int exit_code)
        {
            if (ended_pid != pid)
                // Filter PIDs that aren't ours. (Shouldn't ever happen, but isn't worth logging.)
                    return;

                /*
                 * We would like to keep to RAII as much as practicable, but the streams must be irrevocably reset upon
                 * sub-process termination otherwise the entire thread will hang.
                 */
                this->stdout_stream.reset();
                this->stderr_stream.reset();
                Glib::spawn_close_pid(pid);

                if (exit_code == 0)
                    logger->info("Subprocess with PID " + std::to_string(pid) + " exited normally.");
                else
                    logger->warn("Subprocess with PID " + std::to_string(pid) + " exited with non-zero exit code " +
                            std::to_string(exit_code) + '.');

                finished_callback(exit_code);
            },
            pid
        );

    return { stdout_fd, stderr_fd };
}

} // namespace optifol
