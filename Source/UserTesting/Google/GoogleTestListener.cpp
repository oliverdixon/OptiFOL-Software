/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/10/25.
//

#include <giomm/inetsocketaddress.h>
#include <giomm/socketlistener.h>
#include <iostream>
#include <ranges>

#include "../../Logging.hpp"
#include "GoogleTestListener.hpp"

namespace optifol
{

const log4cxx::LoggerPtr GoogleTestListener::logger = Logging::get_logger({"Network", "GoogleTestListener"});

std::istringstream GoogleTestListener::lexer_input_stream;
GoogleTestLexer GoogleTestListener::lexer{GoogleTestListener::lexer_input_stream, std::cerr}; // TODO
GoogleTestParser GoogleTestListener::parser{&GoogleTestListener::lexer};

GoogleTestListener::GoogleTestListener()
{
    try {
        const auto address = Gio::InetAddress::create_loopback(Gio::SocketFamily::IPV4);
        const auto socket_address = Gio::InetSocketAddress::create(address, 12345);
        Glib::RefPtr<Gio::SocketAddress> effective_address;

        listener->add_address(socket_address, Gio::Socket::Type::STREAM, Gio::Socket::Protocol::TCP, effective_address);
        listener->accept_async(sigc::mem_fun(*this, &GoogleTestListener::connection_callback));

        logger->info("Opened listening TCP socket on " + effective_address->to_string());
    } catch (const Glib::Error &exception) {
        logger->error("Cannot open TCP socket for listening.");
        logger->error(exception.what());
        return;
    }
}

void GoogleTestListener::connection_callback(const Glib::RefPtr<Gio::AsyncResult> &result)
{
    logger->debug("Connection callback triggered; attempting to decode client and read bytes.");

    try {
        const auto connection = listener->accept_finish(result);
        logger->info("Accepted TCP connection from " + connection->get_remote_address()->to_string());

        const auto input_stream = connection->get_input_stream();
        char buffer[1024];

        gssize bytes_read = 0; // TODO use read_async
        while ((bytes_read = input_stream->read(buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            logger->info("Read " + std::to_string(bytes_read) + " from input stream of last connection.");
            read_line(buffer);
        }

        listener->accept_async(sigc::mem_fun(*this, &GoogleTestListener::connection_callback));
    } catch (const Glib::Error &exception) {
        logger->error("Cannot accept or read from client on TCP socket.");
        logger->error(exception.what());
    }
}

void GoogleTestListener::read_line(const char *line)
{
    lexer_input_stream.str(line);
    parser.parse();
}

} // namespace optifol
