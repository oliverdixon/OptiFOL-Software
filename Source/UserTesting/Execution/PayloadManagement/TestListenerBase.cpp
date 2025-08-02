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
#include "../../../Storage/Requirement.hpp"

namespace optifol
{

void TestListenerBase::accept_result(std::unique_ptr<TestResult> &&test_result)
{
    received_test_blob.emplace(std::move(test_result));
}

void TestListenerBase::endow_test(Test &candidate)
{
    // TestResults are keyed on the fixture and test name. If a Test demands one that is in our blob, share it.
    const auto result_it = received_test_blob.find(std::make_pair(
        candidate.property_fixture().get_value(),
        candidate.property_name().get_value()
    ));

    if (result_it != received_test_blob.cend())
        candidate.emplace_result(*result_it);

    // TODO log.
}

} // namespace optifol
