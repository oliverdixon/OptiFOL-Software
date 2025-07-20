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

namespace optifol
{

const log4cxx::LoggerPtr ProcessExecutor::logger = Logging::get_logger({"SubprocessControl", "TextExecutor", "Core"});

ProcessExecutor::ProcessExecutor(const std::string &working_directory, const std::vector<std::string> &argv,
        const std::vector<std::string> &envp, const Glib::RefPtr<Gtk::TextBuffer> &output,
        sigc::slot<void(int)>&& finished_callback) :
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

    int stdout_fd;
    int stderr_fd;

    Glib::spawn_async_with_pipes(working_directory, argv, envp,
            Glib::SpawnFlags::SEARCH_PATH | Glib::SpawnFlags::DO_NOT_REAP_CHILD, {}, &pid, nullptr, &stdout_fd,
            &stderr_fd);

    Glib::signal_child_watch().connect(
        [this, finished_callback](const Glib::Pid ended_pid, const int exit_code)
        {
            if (ended_pid == pid)
                // Filter PIDs that aren't ours. (Shouldn't ever happen.)
                finished_callback(exit_code);
        },
        pid
    );

    stdout_stream.emplace(stdout_tag, stdout_fd, sigc::mem_fun(*this, &ProcessExecutor::stream_callback));
    stderr_stream.emplace(stderr_tag, stderr_fd, sigc::mem_fun(*this, &ProcessExecutor::stream_callback));

    logger->info("Spawned subprocess \"" + argv[0] + "\" with PID " + std::to_string(pid) + '.');
}

ProcessExecutor::Stream::Stream(const Glib::RefPtr<Gtk::TextTag> &formatting_tag, const int source_fd,
        sigc::bound_mem_functor<decltype(&ProcessExecutor::stream_callback), Glib::IOCondition, const Stream *>&&
            write_line_callback) :
    formatting_tag(formatting_tag),
    channel(Glib::IOChannel::create_from_fd(source_fd)),
    watch(Glib::signal_io().connect(
        sigc::bind(std::move(write_line_callback), this),
        channel,
        Glib::IOCondition::IO_IN | Glib::IOCondition::IO_HUP | Glib::IOCondition::IO_ERR
    ))
{
}

ProcessExecutor::Stream::~Stream()
{
    if (channel != nullptr)
        channel->close();

    if (watch.connected())
        watch.disconnect();
}

void ProcessExecutor::Stream::append_line_to_buffer(const Glib::RefPtr<Gtk::TextBuffer> &target_buffer) const
{
    Glib::ustring line;
    if (channel->read_line(line) == Glib::IOStatus::NORMAL)
        target_buffer->insert_with_tag(target_buffer->end(), line, formatting_tag);
}

bool ProcessExecutor::stream_callback(const Glib::IOCondition condition, const Stream *stream_metadata) const
{
    if ((condition & Glib::IOCondition::IO_IN) == Glib::IOCondition::IO_IN) {
        stream_metadata->append_line_to_buffer(output);
        return true;
    }

    /*
     * If we've triggered the callback with something other than a IOCondition::IO_IN, something unexpected has
     * happened and Glib is indicating an error state.
     */
    // TODO handle.
    return false;
}

} // namespace optifol
