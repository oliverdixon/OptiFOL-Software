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

#include <ranges>

#include "../Exceptions/ParseError.hpp"
#include "../Exceptions/SemanticException.hpp"
#include "Test.hpp"

namespace optifol
{

Test::Test(const std::string_view packed_input_line) :
    Glib::ObjectBase("Test"),
    target_executable(*this, "Test-target-executable"),
    test_suite(*this, "Test-test-suite"),
    test_name(*this, "Test-test-name"),
    result(*this, "Test-result")
{
    using std::operator""sv;
    static constexpr auto delimeter{":"sv};

    auto tokens = std::views::split(packed_input_line, delimeter);
    auto it = tokens.cbegin();

    if (it == tokens.cend())
        throw ParseError("No target executable provided in unit test specification \"" + std::string(packed_input_line)
            + "\".", 0);
    target_executable.set_value(std::string(std::string_view(*it++)));

    if (it == tokens.cend())
        throw ParseError("No test suite provided in unit test specification \"" + std::string(packed_input_line)
            + "\".", 0);
    test_suite.set_value(std::string(std::string_view(*it++)));

    if (it == tokens.cend())
        throw ParseError("No test name provided in unit test specification \"" + std::string(packed_input_line)
            + "\".", 0);
    test_name.set_value(std::string(std::string_view(*it++)));

    if (it != tokens.cend())
        throw ParseError("Additional parameters provided in unit test specification \"" + std::string(packed_input_line)
            + "\".", 0);
}

Test::Test(std::string target_executable, std::string test_suite, std::string test_name) :
    Glib::ObjectBase("Test"),
    target_executable(*this, "Test-target-executable", std::move(target_executable)),
    test_suite(*this, "Test-test-suite", std::move(test_suite)),
    test_name(*this, "Test-test-name", std::move(test_name)),
    result(*this, "Test-result")
{
}

Test::Test(std::string target_executable, std::string test_suite, std::string test_name, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Test"),
    StorageObjectBase(cobject, builder),
    target_executable(*this, "Test-target-executable", std::move(target_executable)),
    test_suite(*this, "Test-test-suite", std::move(test_suite)),
    test_name(*this, "Test-test-name", std::move(test_name)),
    result(*this, "Test-result")
{
}

void Test::emplace_result(const std::shared_ptr<TestResult> &test_result)
{
    const auto& result_suite_value = test_result->get_test_suite_name();
    const auto& expected_suite_value = test_suite.get_value();

    if (expected_suite_value.has_value() != result_suite_value.has_value())
        throw SemanticException("Incoming test result does not match expected suite-test structure.");

    if (result_suite_value.has_value() && *result_suite_value != Glib::UStringView(*expected_suite_value))
        throw SemanticException("Incoming test result was from a different suite: \"" + *result_suite_value +
            "\", but needed \"" + *expected_suite_value + "\".");

    if (test_result->get_test_name() != Glib::UStringView(test_name))
        throw SemanticException("Incoming test result was from a different test: \"" + test_result->get_test_name() +
            "\", but needed \"" + test_name + "\".");

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
