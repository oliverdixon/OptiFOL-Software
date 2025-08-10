/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the TestResult results storage
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#include "TestResult.hpp"

#include <cassert>
#include <utility>

namespace optifol
{

TestResult::TestResult(Glib::ustring test_name, const bool passed, const std::size_t execution_time,
        std::vector<Glib::RefPtr<PartialTestResult>> &&partial_results) :
    test_name(std::move(test_name)),
    passed(passed),
    execution_time(execution_time)
{
    for (const auto& partial_result : partial_results)
        this->partial_results->append(partial_result);
}

std::size_t TestResult::hash() const noexcept
{
    return hash_combine(std::hash<std::string>{}(fixture_name), std::hash<std::string>{}(test_name));
}

Glib::RefPtr<Gtk::TreeListModel> TestResult::get_tests_tree() const noexcept
{
    return nullptr;
}

Glib::RefPtr<Gtk::TreeListModel> TestResult::get_results_tree() const noexcept
{
    return partial_results_tree;
}

void TestResult::populate_test_fixture_name(const std::string &incoming_fixture_name)
{
    fixture_name = incoming_fixture_name;
}

Glib::ustring TestResult::copy_test_name() const noexcept
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

Glib::ustring TestResult::copy_fixture_name() const noexcept
{
    return fixture_name;
}

bool TestResult::operator==(const TestResult &other) const noexcept
{
    return test_name == other.test_name && fixture_name == other.fixture_name;
}

bool TestResult::operator==(const std::pair<Glib::ustring, Glib::ustring>& names) const noexcept
{
    return names.first == fixture_name && names.second == test_name;
}

} // namespace optifol
