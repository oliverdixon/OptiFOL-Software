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

void TestListenerBase::endow_requirement(const Requirement & requirement)
{
    const auto requirement_tests = requirement.get_tests();
    const auto test_count = requirement_tests->get_n_items();

    for (guint test_index = 0; test_index < test_count; ++test_index) {
        const auto& test = requirement_tests->get_item(test_index);
        if (test == nullptr)
            continue;

        // TestResults are keyed on the fixture and test name. If a Test demands one that is in our blob, share it.
        const auto result_it = received_test_blob.find(std::make_pair(
            test->property_fixture().get_value(),
            test->property_name().get_value()
        ));

        if (result_it != received_test_blob.cend())
            test->emplace_result(*result_it);
    }
}

} // namespace optifol
