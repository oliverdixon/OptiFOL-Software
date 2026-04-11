/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the framework-agnostic execution group
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#include "ExecutionGroup.hpp"
#include "Test.hpp"

namespace optifol
{

bool ExecutionGroup::operator==(const TestExecutable &other) const
{
    return other.property_name().get_value() == executable.property_name().get_value();
}

bool ExecutionGroup::operator==(const ExecutionGroup &other) const
{
    return other.executable == executable;
}

Glib::ustring ExecutionGroup::get_executable_name() const
{
    return executable.property_name().get_value();
}

ExecutionGroup::ExecutionGroup(const TestExecutable &executable) :
    executable(executable)
{
}

bool ExecutionGroup::check_eligibility(const Test &test) const
{
    return *test.observe_test_executable() == executable;
}

std::size_t ExecutionGroup::hash() const noexcept
{
    return executable.hash();
}

} // namespace optifol

namespace std
{

size_t hash<optifol::ExecutionGroup>::operator()(const optifol::ExecutionGroup &object) const noexcept
{
    return object.hash();
}

size_t hash<optifol::ExecutionGroup>::operator()(const optifol::TestExecutable &executable) const noexcept
{
    return executable.hash();
}

size_t hash<optifol::ExecutionGroup>::operator()(
        const unique_ptr<optifol::ExecutionGroup> &unique_hashable) const noexcept
{
    return unique_hashable->hash();
}

} // namespace std
