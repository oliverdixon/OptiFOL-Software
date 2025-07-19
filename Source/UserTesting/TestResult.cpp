/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/16/25.
//

#include "TestResult.hpp"

#include <cassert>
#include <utility>

namespace optifol
{

TestResult::Partial::Partial(const std::string &file, const std::size_t line, const std::string &message) :
    file(file),
    line(line),
    message(message)
{
}

TestResult::TestResult(std::string test_name, const bool passed, const std::size_t execution_time,
        std::vector<Partial> &&partial_results) :
    test_name(std::move(test_name)),
    passed(passed),
    execution_time(execution_time),
    partial_results(std::move(partial_results))
{
}

std::size_t TestResult::hash() const noexcept
{
    assert(suite_name.has_value());
    return hash_combine(std::hash<std::string>{}(*suite_name), std::hash<std::string>{}(test_name));
}

void TestResult::populate_test_suite_name(std::string suite_name)
{
    this->suite_name.emplace(std::move(suite_name));
}

std::string TestResult::get_test_name() const noexcept
{
    return test_name;
}

bool TestResult::has_passed() const noexcept
{
    return passed;
}

std::size_t TestResult::get_execution_time() const noexcept
{
    return execution_time;
}

std::vector<TestResult::Partial> &&TestResult::steal_partial_results() noexcept
{
    return std::move(partial_results);
}

std::optional<std::string> TestResult::get_test_suite_name() const noexcept
{
    return suite_name;
}

bool TestResult::operator==(const TestResult &other) const noexcept
{
    return test_name == other.test_name && suite_name == other.suite_name;
}

bool TestResult::operator==(const TestResult *other) const noexcept
{
    return *this == *other;
}

bool TestResult::operator==(const std::pair<std::string_view, std::string_view>& names) const noexcept
{
    if (suite_name.has_value() == false)
        return names.first.empty() && names.second == test_name;

    return names.first == *suite_name && names.second == test_name;
}

} // namespace optifol
