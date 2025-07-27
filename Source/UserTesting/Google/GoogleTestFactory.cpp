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

#include "GoogleTestFactory.hpp"
#include "../GUI/ProcessExecutor.hpp"

namespace optifol
{

std::unique_ptr<ProcessExecutor> GoogleTestFactory::execute_tests(sigc::slot<void(int)> &&finished_callback)
{
    return std::make_unique<ProcessExecutor>(
        "",
        std::vector<std::string>{
            "cmake-build-debug/OptifolTesting",
            "--gtest_filter=FOLParserTest.*", // TODO get from selected requirement/test group.
            "--gtest_stream_result_to=127.0.0.1:12345"
        },
        std::vector<std::string>{},
        std::move(finished_callback)
    );
}

std::unique_ptr<ProcessExecutor> GoogleTestFactory::dry_run_executable(const std::string_view executable_name,
        Glib::RefPtr<Gtk::TextBuffer> output_buffer, sigc::slot<void(int)> &&finished_callback)
{
    return std::make_unique<ProcessExecutor>(
        "",
        std::vector<std::string>{std::string(executable_name), "--gtest_list_tests" },
        std::vector<std::string>{},
        std::move(output_buffer),
        std::move(finished_callback)
    );
}

} // namespace optifol
