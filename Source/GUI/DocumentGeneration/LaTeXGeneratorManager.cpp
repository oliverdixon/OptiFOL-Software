/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/7/25.
//

// TODO not documenting this because it should probably be deleted...

#include "LaTeXGeneratorManager.hpp"

#include <glibmm/main.h>

namespace optifol
{

LaTeXGeneratorManager::LaTeXGeneratorManager(
        const Glib::RefPtr<Gtk::TextBuffer> &buffer, const int fd, const Glib::RefPtr<Gtk::TextTag>& formatting_tag) :
    buffer(buffer),
    formatting_tag(formatting_tag),
    channel(Glib::IOChannel::create_from_fd(fd)),
    watch(Glib::signal_io().connect(sigc::mem_fun(*this, &LaTeXGeneratorManager::stream_callback), channel,
            Glib::IOCondition::IO_IN | Glib::IOCondition::IO_HUP))
{
}

LaTeXGeneratorManager::~LaTeXGeneratorManager()
{
    watch.disconnect();
    channel.reset();
}

bool LaTeXGeneratorManager::stream_callback(const Glib::IOCondition condition)
{
    if ((condition & Glib::IOCondition::IO_IN) == Glib::IOCondition::IO_IN) {
        Glib::ustring line;
        const Glib::IOStatus status = channel->read_line(line);
        if (status == Glib::IOStatus::NORMAL)
            buffer->insert_with_tag(buffer->end(), line, formatting_tag);

        return true;
    }

    if ((condition & Glib::IOCondition::IO_HUP) == Glib::IOCondition::IO_HUP) {
        watch.disconnect();
        channel.reset();
    }

    return false;
}

} // namespace optifol
