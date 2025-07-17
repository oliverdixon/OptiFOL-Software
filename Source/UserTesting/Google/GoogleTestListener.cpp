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

const log4cxx::LoggerPtr GoogleTestListener::logger = Logging::get_logger({"UserTesting", "GoogleTestListener"});

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

        std::string results_string;
        char buffer[1024];

        gssize bytes_read = 0; // TODO use read_async
        while ((bytes_read = input_stream->read(buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            logger->debug("Read " + std::to_string(bytes_read) + " from input stream of last connection.");
            results_string += buffer;
        }

        listener->accept_async(sigc::mem_fun(*this, &GoogleTestListener::connection_callback));

        std::istringstream lexer_input_stream;
        GoogleTestLexer lexer{lexer_input_stream, std::cerr};
        GoogleTestParser parser{&lexer};

        lexer_input_stream.str(results_string);
        parser.parse();
        auto test_results = parser.steal_test_results();
        // TODO do something with results
    } catch (const Glib::Error &exception) {
        logger->error("Cannot accept or read from client on TCP socket.");
        logger->error(exception.what());
    } catch (const SemanticException&) {
        logger->error("Semantic error during parse: the packets were received and complied with the protocol schema, "
                      "but were meaningless.");
    } catch (const ParseError&) {
        logger->error("Parse error during reception: the packets were received, but did not comply with the schema.");
    }
}

} // namespace optifol
