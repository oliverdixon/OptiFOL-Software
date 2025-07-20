/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the process streaming and control mechanism
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#include <glibmm/main.h>

#include "ProcessStream.hpp"
#include "../Logging.hpp"

namespace optifol
{

const log4cxx::LoggerPtr ProcessStream::logger = Logging::get_logger("SubprocessControl");

ProcessStream::ProcessStream(const Glib::RefPtr<Gtk::TextTag> &formatting_tag) :
    formatting_tag(formatting_tag)
{
}

ProcessStream::~ProcessStream()
{
    disconnect();
}

void ProcessStream::connect(const int source_fd, const sigc::slot<bool(Glib::IOCondition)> &callback_slot)
{
    logger->debug("Attaching to process output buffer with descriptor " +
            std::to_string(source_fd) + '.');

    channel = Glib::IOChannel::create_from_fd(source_fd);
    watch = Glib::signal_io().connect(
            callback_slot, channel, Glib::IOCondition::IO_IN | Glib::IOCondition::IO_HUP | Glib::IOCondition::IO_ERR);
}

void ProcessStream::disconnect()
{
    logger->debug("Disconnecting from subprocess channel.");

    if (channel != nullptr)
        channel->close();

    if (watch.connected())
        watch.disconnect();
}

bool ProcessStream::is_connected() const noexcept
{
    return watch.connected();
}

void ProcessStream::append_line_to_buffer(const Glib::RefPtr<Gtk::TextBuffer> &target_buffer) const
{
    Glib::ustring line;
    if (channel->read_line(line) == Glib::IOStatus::NORMAL)
        target_buffer->insert_with_tag(target_buffer->end(), line, formatting_tag);
}

} // namespace optifol
