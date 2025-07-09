/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the process streaming and control mechanism
 * @author Oliver Dixon
 * @date 2025-07-09
 * @version Development
 */

#ifndef PROCESSSTREAM_HPP
#define PROCESSSTREAM_HPP

#include <glibmm/iochannel.h>
#include <glibmm/refptr.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/texttag.h>
#include <log4cxx/logger.h>

namespace optifol
{

/**
 * @class ProcessStream
 * @brief RAII wrapper for a connection to the streaming data buffer (nominally a pipe or socket) of an external
 *  process, with support for writing formatted data to a Gtk::TextBuffer.
 */
struct ProcessStream
{
    /**
     * @brief Create a new empty ProcessStream
     * @param formatting_tag The constant formatting tag to use for line formatting of the stream data in
     *  Gtk::TextView windows.
     */
    explicit ProcessStream(const Glib::RefPtr<Gtk::TextTag> & formatting_tag);

    /**
     * @brief RAII-destruct the ProcessStream by disconnecting any system/subprocess references.
     */
    ~ProcessStream();

    /**
     * @brief Prevent copying as it causes unnecessary complexity and is not semantically necessary.
     */
    ProcessStream(const ProcessStream&) = delete;

    /**
     * @brief Prevent moving as it causes unnecessary complexity and is not semantically necessary.
     * @see @ref disconnect()
     */
    ProcessStream(ProcessStream&&) = delete;

    /**
     * @brief Connect a new FD to the ProcessStream slot
     * @param source_fd The numerical file descriptor of the stream to watch
     * @param callback_slot The callback for events on the stream identified by the given descriptor
     */
    void connect(int source_fd, const sigc::slot<bool(Glib::IOCondition)> &callback_slot);

    /**
     * @brief Disconnect the stream, releasing all held resources except the formatting tag shared reference.
     */
    void disconnect();

    /**
     * @brief Query the connection state of the ProcessStream slot
     * @return Does the ProcessStream hold an active connection?
     */
    [[nodiscard]] bool is_connected() const noexcept;

    /**
     * @brief Write the latest line from the stream to the end of the given Gtk::TextBuffer with the stream's
     *  formatting tag
     * @param target_buffer The target buffer to which data should be appended
     * @warning There is a known issue involving multiple ChannelStream objects being used to post to the same
     *  Gtk::TextBuffer. In particular, as they are asynchronous, output lines can become unordered during the data
     *  race. Where streams are formatted differently, this is very noticeable by a user. The solution is to push
     *  all incoming lines into a shared queue, preserving the order, that is periodically flushed to the
     *  Gtk::TextBuffer/Gtk::TextView, but this is a relatively low priority fix.
     */
    void append_line_to_buffer(const Glib::RefPtr<Gtk::TextBuffer>& target_buffer) const;

    const Glib::RefPtr<Gtk::TextTag> formatting_tag;

private:
    static const log4cxx::LoggerPtr logger;

    sigc::connection watch;
    Glib::RefPtr<Glib::IOChannel> channel;
};

} // namespace optifol

#endif // PROCESSSTREAM_HPP
