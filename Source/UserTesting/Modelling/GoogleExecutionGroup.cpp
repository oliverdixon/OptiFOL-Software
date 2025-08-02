/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 8/2/25.
//

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
    // TODO URGENT: what if executable doesn't exist? How to express on UI?

    /*
     * TODO: if the binding fails due to the port number being taken, we should continue to try until we (a) hit the
     *  max, or (b) find an unused port and bind successfully. The bound port may not be the same as the one passed, so
     *  TestListenerBase should provide functionality to interrogate the effective address of the socket once bound.
     */
    listener = std::make_unique<GoogleTestListener>(port_number);
    executor = std::make_unique<ProcessExecutor>(
        "",
        std::vector<std::string>{
            get_executable_name(),
            "--gtest_filter=" + filter_line,
            "--gtest_stream_result_to=127.0.0.1:" + std::to_string(port_number)
        },
        std::vector<std::string>{},
        sigc::mem_fun(*this, &GoogleExecutionGroup::distribute_results)
    );

    if (++port_number == maximum_port_number) {
        port_number = minimum_port_number;
        // TODO log warning
    }
}

void GoogleExecutionGroup::add_test(Glib::RefPtr<Test> new_test)
{
    if (check_eligibility(*new_test) == false)
        throw SemanticException("The Test does not match to the GoogleExecutionGroup.");

    decltype(filter_line)::const_iterator before_it;

    if (filter_line.empty())
        before_it = filter_line.cbegin();
    else
        before_it = --filter_line.cend();

    filter_line += new_test->property_fixture().get_value() + component_separator +
        new_test->property_name().get_value() + pattern_separator;

    tests.emplace(std::move(new_test), std::make_pair(before_it, --filter_line.cend()));
}

void GoogleExecutionGroup::remove_test(Glib::RefPtr<Test> target_test)
{
    const auto it = tests.find(std::move(target_test));
    if (it != tests.cend()) {
        filter_line.replace(it->second.first, it->second.second, {});
        tests.erase(it);
    }
}

void GoogleExecutionGroup::distribute_results(const int exit_code) const
{
    std::ignore = exit_code;

    for (auto& test : tests)
        listener->endow_test(*test.first);
}

} // namespace optifol
