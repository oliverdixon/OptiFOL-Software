/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/23/25.
//

// ReSharper disable CppMemberFunctionMayBeConst - Do not want to mutate model from immutable group context.

#include <cassert>

#include "RequirementGroupBase.hpp"

namespace optifol
{

const Gio::ListStore<Requirement> &RequirementGroupBase::observe_requirements() const noexcept
{
    assert(requirements != nullptr);
    return *requirements;
}

Glib::RefPtr<Gio::ListStore<Requirement>> &RequirementGroupBase::get_mutable_list() noexcept
{
    return requirements;
}

void RequirementGroupBase::insert(const Glib::RefPtr<Requirement> &requirement)
{
    requirements->append(requirement);
}

void RequirementGroupBase::insert(const std::vector<Glib::RefPtr<Requirement>> &new_requirement)
{
    this->requirements->splice(0, 0, new_requirement);
}

void RequirementGroupBase::insert(std::vector<Glib::RefPtr<Requirement>> &&new_requirements)
{
    this->requirements->splice(0, 0, new_requirements);
}

} // namespace optifol
