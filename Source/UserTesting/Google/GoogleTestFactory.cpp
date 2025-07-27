/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for Google Test framework factories.
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#include <vector>

#include "../../GUI/StreamingProcessExecutor.hpp"
#include "../../Storage/TestGroup.hpp"
#include "GoogleTestFactory.hpp"
#include "GoogleTestListener.hpp"

namespace optifol
{

const log4cxx::LoggerPtr GoogleTestFactory::logger = Logging::get_logger({"UserTesting", "GoogleTest", "Factories"});
guint16 GoogleTestFactory::port_number = GoogleTestFactory::minimum_port_number;

std::pair<std::unique_ptr<ProcessExecutor>, std::unique_ptr<TestListenerBase>> GoogleTestFactory::execute_test_group(
    const std::string_view test_executable,
    const std::string_view test_specification,
    sigc::slot<void(std::unique_ptr<TestResult>&&)>&& new_result_callback,
    sigc::slot<void()>&& results_finished_callback,
    sigc::slot<void(int)> &&process_finished_callback
)
{
    /*
     * TODO: if the binding fails due to the port number being taken, we should continue to try until we (a) hit the
     *  max, or (b) find an unused port and bind successfully. The bound port may not be the same as the one passed, so
     *  TestListenerBase should provide functionality to interrogate the effective address of the socket once bound.
     */
    auto listener = std::make_unique<GoogleTestListener>(std::move(new_result_callback),
        std::move(results_finished_callback), port_number);

    auto executor = std::make_unique<ProcessExecutor>(
        "",
        std::vector{
            std::string(test_executable),
            "--gtest_filter=" + std::string(test_specification),
            "--gtest_stream_result_to=127.0.0.1:" + std::to_string(port_number)
        },
        std::vector<std::string>{},
        std::move(process_finished_callback)
    );

    if (++port_number == maximum_port_number) {
        port_number = minimum_port_number;
        logger->warn("Socket port number reached maximum value. Wrapped back to " + std::to_string(port_number) + '.');
    }

    return { std::move(executor), std::move(listener) };
}

std::unique_ptr<ProcessExecutor> GoogleTestFactory::dry_run_executable(const std::string_view executable_name,
    Glib::RefPtr<Gtk::TextBuffer> output_buffer, sigc::slot<void(int)> &&finished_callback)
{
    return std::make_unique<StreamingProcessExecutor>(
        "",
        std::vector<std::string>{std::string(executable_name), "--gtest_list_tests" },
        std::vector<std::string>{},
        std::move(output_buffer),
        std::move(finished_callback)
    );
}

} // namespace optifol
