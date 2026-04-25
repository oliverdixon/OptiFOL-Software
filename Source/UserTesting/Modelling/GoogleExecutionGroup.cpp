/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Google Test execution group
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#include "GoogleExecutionGroup.hpp"

#include "../../Exceptions/SemanticException.hpp"
#include "GoogleTestListener.hpp"
#include "Test.hpp"

namespace optifol
{

guint16 GoogleExecutionGroup::port_number = GoogleExecutionGroup::minimum_port_number;

GoogleExecutionGroup::GoogleExecutionGroup(Glib::RefPtr<Test> initial_test) :
    ExecutionGroup(*initial_test->observe_test_executable())
{
    GoogleExecutionGroup::add_test(std::move(initial_test));
}

void GoogleExecutionGroup::run()
{
    if (is_empty())
        return;

    if (cache_ok == false)
        invalidate_cache();

    listener = std::make_unique<GoogleTestListener>(port_number);
    executor = std::make_unique<ProcessExecutor>("",
            std::vector<std::string>{get_executable_name(), "--gtest_filter=" + filter_line_cache,
                    "--gtest_stream_result_to=127.0.0.1:" + std::to_string(port_number)},
            std::vector<std::string>{}, sigc::mem_fun(*this, &GoogleExecutionGroup::distribute_results));

    if (++port_number == maximum_port_number)
        port_number = minimum_port_number;
}

void GoogleExecutionGroup::add_test(Glib::RefPtr<Test> new_test)
{
    if (check_eligibility(*new_test) == false)
        throw SemanticException("The Test does not match to the GoogleExecutionGroup.");

    tests.push_front(std::move(new_test));
    cache_ok = false;
}

void GoogleExecutionGroup::remove_test(const Glib::RefPtr<Test> &target_test)
{
    tests.remove(target_test);
    cache_ok = false;
}

std::size_t GoogleExecutionGroup::is_empty() const noexcept
{
    return tests.empty();
}

void GoogleExecutionGroup::distribute_results(const int exit_code) const
{
    std::ignore = exit_code;
    for (auto &test: tests)
        listener->endow_test(*test);
}

void GoogleExecutionGroup::invalidate_cache()
{
    filter_line_cache.clear();

    for (const auto &test: tests)
        filter_line_cache +=
                test->property_fixture().get_value() + '.' + test->property_name().get_value() + ':';

    cache_ok = true;
}

} // namespace optifol
