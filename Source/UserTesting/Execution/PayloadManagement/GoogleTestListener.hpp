/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Google Test TCP socket listener
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef GOOGLETESTLISTENER_HPP
#define GOOGLETESTLISTENER_HPP

#include <giomm/socketlistener.h>
#include <log4cxx/logger.h>

#include "GoogleTestLexer.hpp"
#include "TestListenerBase.hpp"

namespace optifol
{

class TestResult;

/**
 * @class GoogleTestListener
 * @brief Accept incoming packets from the Google Test testing framework, parse the payloads into TestResult
 * objects, and invoke the listening callbacks.
 *
 * @details The constructed class opens a non-blocking listener, bound to the IPv4 localhost address
 * 127.0.0.1, on a single implementation-defined port number. Client connections on TCP are accepted and
 * continue to be read into a large per-connection internal buffer until the connection is closed on the
 * socket. Individual TestResult objects are confined into an owning container and stored internally. Once the
 * streaming has completed, callers can execute
 *  @ref TestListenerBase::endow_test to provide ref-counted pointers to Test objects relevant to the streamed
 *  TestResult records.
 *
 * @see @ref GoogleTestLexer.l for the expected token format of the Google Test Payload
 * @see @ref GoogleTestParser.y for the expected grammar of the Google Test payload
 */
class GoogleTestListener : public TestListenerBase
{
public:
    /**
     * @brief Construct the listener by opening a new listening socket and registering callbacks.
     * @param port_number The port number of the listening socket.
     * @throws Glib::Error The socket could not be established.
     */
    explicit GoogleTestListener(guint16 port_number);

private:
    void connection_callback(const Glib::RefPtr<Gio::AsyncResult> &result) noexcept override;

    static const log4cxx::LoggerPtr logger;
    static constexpr std::size_t temp_buffer_size = 1024;

    Glib::RefPtr<Gio::SocketListener> listener = Gio::SocketListener::create();

    std::istringstream lexer_input_stream;
    GoogleTestLexer lexer{lexer_input_stream, std::cerr}; // TODO do not use std::cerr
    GoogleTestParser parser;
};

} // namespace optifol

#endif // GOOGLETESTLISTENER_HPP
