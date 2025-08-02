/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for Google Test framework factories.
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#ifndef GOOGLETESTFACTORY_HPP
#define GOOGLETESTFACTORY_HPP

#include <gtkmm/textbuffer.h>
#include <log4cxx/logger.h>
#include <memory>

namespace optifol
{

class TestListenerBase;
class TestResult;
class ProcessExecutor;

class GoogleTestFactory
{
public:
    static std::pair<std::unique_ptr<ProcessExecutor>, std::unique_ptr<TestListenerBase>> execute_test_group(
        std::string_view test_executable,
        std::string_view test_specification,
        sigc::slot<void(int)> &&process_finished_callback
    );

private:
    static const log4cxx::LoggerPtr logger;
    static constexpr guint16 minimum_port_number = 1024; // First unprivileged port number under Linux.
    static constexpr guint16 maximum_port_number = std::numeric_limits<guint16>::max();
    static guint16 port_number;
};

} // namespace optifol

#endif // GOOGLETESTFACTORY_HPP
