/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/16/25.
//

#include "TestResult.hpp"

namespace optifol
{

TestResult::Partial::Partial(const std::string &file, std::size_t line, const std::string &message) :
    file(file),
    line(line),
    message(message)
{
}

TestResult::TestResult(const bool passed, const std::size_t execution_time, std::vector<Partial> &&partial_results) :
    Glib::ObjectBase("TestResult"),
    passed(*this, "TestResult-passed", passed),
    execution_time(*this, "TestResult-execution-time", execution_time),
    partial_results(*this, "TestResult-partial-results", std::move(partial_results))
{
}

TestResult::TestResult(const bool passed, const std::size_t execution_time, std::vector<Partial> &&partial_results,
        BaseObjectType *const cobject, const Glib::RefPtr<Gtk::Builder> &) :
    Glib::ObjectBase("TestResult"),
    Glib::Object(cobject),
    passed(*this, "TestResult-passed", passed),
    execution_time(*this, "TestResult-execution-time", execution_time),
    partial_results(*this, "TestResult-partial-results", std::move(partial_results))
{
}

Glib::PropertyProxy<bool> TestResult::property_passed()
{
    return passed.get_proxy();
}

Glib::PropertyProxy_ReadOnly<bool> TestResult::property_passed() const
{
    return passed.get_proxy();
}

Glib::PropertyProxy<std::size_t> TestResult::property_execution_time()
{
    return execution_time.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::size_t> TestResult::property_execution_time() const
{
    return execution_time.get_proxy();
}

Glib::PropertyProxy<std::vector<TestResult::Partial>> TestResult::property_partial_results()
{
    return partial_results.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::vector<TestResult::Partial>> TestResult::property_partial_results() const
{
    return partial_results.get_proxy();
}

} // namespace optifol
