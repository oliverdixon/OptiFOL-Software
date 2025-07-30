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

    const auto &glib_suite_name = test->property_fixture().get_value();
    const auto &glib_test_name = test->property_name().get_value();

#if 0 // TODO URGENT
    // TestResults are keyed on the fixture and test name. If the Requirement demands one that is in our blob, share it.
    const auto it = received_test_blob.find(std::make_pair(
        Glib::UStringView(glib_suite_name),
        Glib::UStringView(glib_test_name)
    ));

    if (it != received_test_blob.cend())
        requirement.emplace_test_result(*it);
#endif
}

} // namespace optifol
