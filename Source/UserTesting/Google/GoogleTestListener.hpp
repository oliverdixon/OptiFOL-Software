/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/10/25.
//

#ifndef GOOGLETESTLISTENER_HPP
#define GOOGLETESTLISTENER_HPP

#include <giomm/socketlistener.h>
#include <log4cxx/logger.h>

#include "../ITestListener.hpp"
#include "GoogleTestLexer.hpp"

namespace optifol
{

class GoogleTestListener : public ITestListener
{
public:
    GoogleTestListener();

private:
    void connection_callback(const Glib::RefPtr<Gio::AsyncResult> &result) override;

    static const log4cxx::LoggerPtr logger;

    static constexpr float gtest_protocol_version = 1.0f;

    static std::istringstream lexer_input_stream;
    static GoogleTestLexer lexer;
    static GoogleTestParser parser;

    Glib::RefPtr<Gio::SocketListener> listener = Gio::SocketListener::create();
};

} // namespace optifol

#endif // GOOGLETESTLISTENER_HPP
