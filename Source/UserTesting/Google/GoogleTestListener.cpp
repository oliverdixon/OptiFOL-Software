/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Google Test TCP socket listener
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#include <giomm/inetsocketaddress.h>
#include <giomm/socketlistener.h>
#include <giomm/resource.h>

#include "../../Logging.hpp"
#include "GoogleTestListener.hpp"

namespace optifol
{

const log4cxx::LoggerPtr GoogleTestListener::logger = Logging::get_logger({"UserTesting", "GoogleTestListener"});

GoogleTestListener::GoogleTestListener(
        sigc::slot<void(std::unique_ptr<TestResult> &&)> &&report_callback, sigc::slot<void()> &&close_callback) :
    TestListenerBase(std::move(report_callback)),
    close_callback(close_callback),
    parser(&lexer, sigc::mem_fun(*this, &TestListenerBase::report_result))
{
    try {
        const auto address = Gio::InetAddress::create_loopback(Gio::SocketFamily::IPV4);
        const auto socket_address = Gio::InetSocketAddress::create(address, 12345);
        Glib::RefPtr<Gio::SocketAddress> effective_address;

        if (listener->add_address(socket_address, Gio::Socket::Type::STREAM, Gio::Socket::Protocol::TCP,
                effective_address) == false)
            throw Gio::ResourceError(Gio::ResourceError::INTERNAL, "Could not bind to " + socket_address->to_string());

        listener->accept_async(sigc::mem_fun(*this, &GoogleTestListener::connection_callback));
        logger->info("Opened listening TCP socket on " + effective_address->to_string());
    } catch (const Glib::Error &exception) {
        logger->error("Cannot open TCP socket for listening.");
        logger->error(exception.what());
        throw;
    }
}

GoogleTestListener::~GoogleTestListener()
{
    close_callback.disconnect();
}

void GoogleTestListener::connection_callback(const Glib::RefPtr<Gio::AsyncResult> &result) noexcept
{
    logger->debug("Connection callback triggered; attempting to decode client and read bytes.");

    try {
        const auto connection = listener->accept_finish(result);
        logger->info("Accepted TCP connection from " + connection->get_remote_address()->to_string());

        const auto input_stream = connection->get_input_stream();

        /*
         * We can make some assertions on the full results string, so std::string::append is likely more performant than
         * std::ostringstream::operator<<. See https://stackoverflow.com/questions/19844858. In particular, we know the
         * following:
         *
         *  1. The parser is non-streaming, so we need to have the entire input stored before passing it to the lexer
         *     and parser routines. When the TCP packets arrive in chunks of GoogleTestListener::temp_buffer_size bytes,
         *     they must be collated into a single source; and
         *
         *  2. The collated buffer will grow by factors of GoogleTestListener::temp_buffer_size per append operation, so
         *     we can do allocation ahead of time.
         */

        std::string results_string;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
        /*
         * Our buffer access is guaranteed to be safe. Access is performed at two points:
         *
         *  1. Gio::InputStream::read mutates the buffer in the maximal half-open interval [0, sizeof(buffer)). This is
         *     guaranteed by documented constraints on the read function; and
         *
         *  2. We manually append a NULL-terminator at the position indicated by the return value of the read function.
         *     This is further guaranteed to be within the half-open interval [0, sizeof(buffer)), hence the access can
         *     occur at most sizeof(buffer) - 1 bytes into the array.
         *
         * The use of an "unsafe" C construct is enforced by the GioMM library. See
         * https://gnome.pages.gitlab.gnome.org/glibmm/classGio_1_1InputStream.html#a42ab2486116d40f6ef828277befb508a.
         */
        char buffer[temp_buffer_size];

        gssize bytes_read = 0;
        while ((bytes_read = input_stream->read(buffer, sizeof(buffer) - 1)) > 0) {
            /*
             * We rather do a single worst-case allocation of the entire temporary buffer than lots of small allocations
             * in the actual number of bytes read. In Google Test streaming, packets are often small and numerous.
             */
            results_string.reserve(results_string.capacity() + temp_buffer_size);
            buffer[bytes_read] = '\0';
            logger->trace("Read " + std::to_string(bytes_read) + " from input stream of last connection.");
            results_string.append(buffer);
        }
#pragma clang diagnostic pop

        listener->accept_async(sigc::mem_fun(*this, &GoogleTestListener::connection_callback));

        /*
         * Push the collated input into a lexer stream, lex and parse, and execute the closed callback to indicate that
         * the client stopped sending data and the connection was closed.
         */
        lexer_input_stream.str(results_string);
        parser.parse();
        close_callback();
    } catch (const Glib::Error &exception) {
        logger->error("Cannot accept or read from client on TCP socket.");
        logger->error(exception.what());
    } catch (const SemanticException &) {
        logger->error("Semantic error during parse: the packets were received and complied with the protocol schema, "
                      "but were meaningless.");
    } catch (const ParseError &) {
        logger->error("Parse error during reception: the packets were received, but did not comply with the schema.");
    }
}

} // namespace optifol
