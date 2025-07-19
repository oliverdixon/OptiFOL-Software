/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/15/25.
//

#include "Test.hpp"

#include <cassert>

namespace optifol
{

Test::Test(const std::string &target_executable, const std::string &test_suite, const std::string &test_name) :
    Glib::ObjectBase("Test"),
    target_executable(*this, "Test-target-executable", target_executable),
    test_suite(*this, "Test-test-suite", test_suite),
    test_name(*this, "Test-test-name", test_name),
    passed(*this, "Test-passed"),
    execution_time_ms(*this, "Test-execution-time-ms"),
    partial_results(*this, "Test-partial-results")
{
}

Test::Test(const std::string &target_executable, const std::string &test_suite, const std::string &test_name,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Test"),
    StorageObjectBase(cobject, builder),
    target_executable(*this, "Test-target-executable", target_executable),
    test_suite(*this, "Test-test-suite", test_suite),
    test_name(*this, "Test-test-name", test_name),
    passed(*this, "Test-passed"),
    execution_time_ms(*this, "Test-execution-time-ms"),
    partial_results(*this, "Test-partial-results")
{
}

void Test::emplace_result(std::unique_ptr<TestResult> &&test_result)
{
    // TODO throw a SemanticException if suite or test name is wrong.

    passed.set_value(test_result->has_passed());
    execution_time_ms.set_value(test_result->get_execution_time());
    partial_results.set_value(test_result->steal_partial_results());
}

Glib::PropertyProxy<Glib::ustring> Test::property_target_executable()
{
    return target_executable.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_target_executable() const
{
    return target_executable.get_proxy();
}

Glib::PropertyProxy<std::optional<Glib::ustring>> Test::property_test_suite()
{
    return test_suite.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Test::property_test_name()
{
    return test_name.get_proxy();
}

Glib::PropertyProxy<std::size_t> Test::property_execution_time_ms()
{
    return execution_time_ms.get_proxy();
}

Glib::PropertyProxy<std::vector<TestResult::Partial>> Test::property_partial_results()
{
    return partial_results.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::optional<Glib::ustring>> Test::property_test_suite() const
{
    return test_suite.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_test_name() const
{
    return test_name.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::size_t> Test::property_execution_time_ms() const
{
    return execution_time_ms.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::vector<TestResult::Partial>> Test::property_partial_results() const
{
    return partial_results.get_proxy();
}

} // namespace optifol
