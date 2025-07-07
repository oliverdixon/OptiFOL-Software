/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/7/25.
//

// TODO not documenting this because it should probably be deleted...

#ifndef LATEXGENERATORMANAGER_HPP
#define LATEXGENERATORMANAGER_HPP
#include <glibmm/iochannel.h>
#include <glibmm/refptr.h>
#include <gtkmm/textbuffer.h>

namespace optifol
{

class LaTeXGeneratorManager
{
public:
    explicit LaTeXGeneratorManager(const Glib::RefPtr<Gtk::TextBuffer>& buffer, int fd,
        const Glib::RefPtr<Gtk::TextTag>& formatting_tag);

    ~LaTeXGeneratorManager();

private:
    bool stream_callback(Glib::IOCondition condition);

    const Glib::RefPtr<Gtk::TextBuffer> buffer;
    const Glib::RefPtr<Gtk::TextTag> formatting_tag;

    Glib::RefPtr<Glib::IOChannel> channel;
    sigc::connection watch;
};

} // namespace optifol

#endif // LATEXGENERATORMANAGER_HPP
