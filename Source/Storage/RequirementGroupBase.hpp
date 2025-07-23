/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/23/25.
//

#ifndef REQUIREMENTGROUPBASE_HPP
#define REQUIREMENTGROUPBASE_HPP

#include <giomm/liststore.h>

#include "Requirement.hpp"

namespace optifol
{

class RequirementGroupBase
{
public:
    const Gio::ListStore<Requirement> &observe_requirements() const noexcept;

    Glib::RefPtr<Gio::ListStore<Requirement>> &get_mutable_list() noexcept;

    void insert(const Glib::RefPtr<Requirement>& requirement);

    void insert(const std::vector<Glib::RefPtr<Requirement>> &new_requirement);

    void insert(std::vector<Glib::RefPtr<Requirement>> &&new_requirements);

protected:
    Glib::RefPtr<Gio::ListStore<Requirement>> requirements = Gio::ListStore<Requirement>::create();
};

} // namespace optifol

#endif // REQUIREMENTGROUPBASE_HPP
