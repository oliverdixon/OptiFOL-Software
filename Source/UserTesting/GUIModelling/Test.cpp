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
#include "../Execution/GoogleTestExecutable.hpp"

namespace optifol
{

Test::Test(std::shared_ptr<TestSpecificationEntry> template_specification) :
    Glib::ObjectBase("Test"),
    target_executable_name(*this, "Test-target-executable-name"),
    fixture(*this, "Test-test-fixture"),
    result(*this, "Test-result")
{
    instantiate_from_specification(std::move(template_specification));
}

Test::Test(std::shared_ptr<TestSpecificationEntry> template_specification, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("Test"),
    StorageObjectBase(cobject, builder),
    target_executable_name(*this, "Test-target-executable-name"),
    fixture(*this, "Test-test-fixture"),
    result(*this, "Test-result")
{
    instantiate_from_specification(std::move(template_specification));
}

Glib::RefPtr<Gtk::TreeListModel> Test::get_tree() const noexcept
{
    return nullptr;
}

void Test::emplace_result(std::shared_ptr<TestResult> test_result)
{
    const auto &given_fixture_name = test_result->copy_fixture_name();
    const auto &expected_fixture_name = fixture.get_value();

    if (given_fixture_name != expected_fixture_name)
        throw SemanticException("Incoming test result was from a different fixture: \"" + given_fixture_name +
                "\", but needed \"" + expected_fixture_name + "\".");

    if (test_result->copy_test_name() != property_name().get_value())
        throw SemanticException("Incoming test result was from a different test: \"" + test_result->copy_test_name() +
                "\", but needed \"" + property_name().get_value() + "\".");

    result.set_value(std::move(test_result));
}

void Test::share_test_executable(std::shared_ptr<TargetTestExecutableBase> shared_exe)
{
    target_executable = std::move(shared_exe);

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

void Test::instantiate_from_specification(std::shared_ptr<TestSpecificationEntry> spec)
{
    share_test_executable(std::make_shared<GoogleTestExecutable>(
            spec->property_executable().get_value()->property_name().get_value()));
    property_fixture().set_value(spec->property_fixture().get_value()->property_name().get_value());
    property_name().set_value(spec->property_name().get_value());

    spec->property_name().signal_changed().connect([this, spec]
    {
        property_name().set_value(spec->property_name().get_value());
    });

    spec->property_fixture().signal_changed().connect([this, spec]
    {
        property_fixture().set_value(spec->property_fixture().get_value()->property_name().get_value());
    });

    // TODO URGENT: executable binding
}

} // namespace optifol
