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

namespace optifol
{

Test::Test(const std::string &target_executable, const std::string &test_suite, const std::string &test_name) :
    Glib::ObjectBase("Test"),
    target_executable(*this, "Test-target-executable", target_executable),
    test_suite(*this, "Test-test-suite", test_suite),
    test_name(*this, "Test-test-name", test_name),
    result(*this, "Test-result")
{
}

Test::Test(const std::string &target_executable, const std::string &test_suite, const std::string &test_name,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Test"),
    StorageObjectBase(cobject, builder),
    target_executable(*this, "Test-target-executable", target_executable),
    test_suite(*this, "Test-test-suite", test_suite),
    test_name(*this, "Test-test-name", test_name),
    result(*this, "Test-result")
{
}

void Test::emplace_result(const std::shared_ptr<TestResult> &test_result)
{
    // TODO throw a SemanticException if suite or test name is wrong.
    result.set_value(test_result);
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

Glib::PropertyProxy<std::shared_ptr<TestResult>> Test::property_result()
{
    return result.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::optional<Glib::ustring>> Test::property_test_suite() const
{
    return test_suite.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> Test::property_test_name() const
{
    return test_name.get_proxy();
}

Glib::PropertyProxy_ReadOnly<std::shared_ptr<TestResult>> Test::property_result() const
{
    return result.get_proxy();
}

} // namespace optifol
