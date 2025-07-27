/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the generic automated test socket listener
 * @author Oliver Dixon
 * @date 2025-07-27
 * @version Development
 */

#include "TestListenerBase.hpp"
#include "../Storage/Requirement.hpp"

namespace optifol
{

void TestListenerBase::accept_result(std::unique_ptr<TestResult> &&test_result)
{
    received_test_blob.emplace(std::move(test_result));
}

void TestListenerBase::endow_requirement(Requirement& requirement)
{
    const auto &test = requirement.observe_test();

    if (test.has_value() == false)
        // Filter out Requirements without associated tests. (Shouldn't ever happen, but isn't worth logging.)
        return;

    const auto &glib_suite_name = test->property_test_suite().get_value();
    const auto &glib_test_name = test->property_name().get_value();

    // TestResults are keyed on the fixture and test name. If the Requirement demands one that is in our blob, share it.
    const auto it = received_test_blob.find(std::make_pair(
        std::string_view(glib_suite_name->c_str(), glib_suite_name->bytes()),
        std::string_view(glib_test_name.c_str(), glib_test_name.bytes())
    ));

    if (it != received_test_blob.cend())
        requirement.emplace_test_result(*it);
}

} // namespace optifol
