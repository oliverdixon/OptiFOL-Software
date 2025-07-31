/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Test-level storage object
 * @author Oliver Dixon
 * @date 2025-07-17
 * @version Development
 */

#include "Test.hpp"
#include "../../Exceptions/SemanticException.hpp"

#include "GoogleTestExecutable.hpp"

namespace optifol
{

Test::Test() :
    Glib::ObjectBase("Test"),
    target_executable_name(*this, "Test-target-executable-name"),
    fixture(*this, "Test-test-suite"),
    result(*this, "Test-result")
{
}

Test::Test(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Test"),
    StorageObjectBase(cobject, builder),
    target_executable_name(*this, "Test-target-executable-name"),
    fixture(*this, "Test-test-suite"),
    result(*this, "Test-result")
{
}

void Test::emplace_result(const std::shared_ptr<TestResult> &test_result)
{
    const auto &result_suite_value = test_result->get_test_suite_name();
    const auto &expected_suite_value = fixture.get_value();

    if (result_suite_value != expected_suite_value)
        throw SemanticException("Incoming test result was from a different suite: \"" + result_suite_value +
                "\", but needed \"" + expected_suite_value + "\".");

    if (test_result->get_test_name() != property_name().get_value().c_str())
        throw SemanticException("Incoming test result was from a different test: \"" + test_result->get_test_name() +
                "\", but needed \"" + property_name().get_value() + "\".");

    result.set_value(test_result);
}

void Test::share_test_executable(std::shared_ptr<TargetTestExecutableBase> shared_exe)
{
    target_executable = shared_exe;

    if (shared_exe == nullptr)
        target_executable_name.set_value("");
    else
        target_executable_name.set_value(target_executable->property_name().get_value());
}

const TargetTestExecutableBase *Test::observe_test_executable() const noexcept
{
    return target_executable.get();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_target_executable_name() const
{
    return target_executable_name.get_proxy();
}

Glib::PropertyProxy<Glib::ustring> Test::property_fixture()
{
    return fixture.get_proxy();
}

Glib::PropertyProxy<std::shared_ptr<TestResult>> Test::property_result()
{
    return result.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_fixture() const
{
    return fixture.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::shared_ptr<TestResult>> Test::property_result() const
{
    return result.get_proxy();
}

} // namespace optifol
