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

class TestResult;

class GoogleTestListener : public ITestListener
{
public:
    explicit GoogleTestListener(sigc::slot<void(std::unique_ptr<TestResult>&&)>&& push_callback,
        sigc::slot<void()>&& close_callback);

private:
    void connection_callback(const Glib::RefPtr<Gio::AsyncResult> &result) override;

    static const log4cxx::LoggerPtr logger;

    Glib::RefPtr<Gio::SocketListener> listener = Gio::SocketListener::create();

    sigc::slot<void()> close_callback;

    std::istringstream lexer_input_stream;
    GoogleTestLexer lexer{lexer_input_stream, std::cerr};
    GoogleTestParser parser;
};

} // namespace optifol

#endif // GOOGLETESTLISTENER_HPP
