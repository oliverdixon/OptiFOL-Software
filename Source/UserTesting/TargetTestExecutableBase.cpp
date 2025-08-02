/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/29/25.
//

#include "TargetTestExecutableBase.hpp"
#include "DiscoveryTestExecutable.hpp"

namespace optifol
{

TargetTestExecutableBase::TargetTestExecutableBase(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    StorageObjectBase(cobject, builder)
{
}

void TargetTestExecutableBase::receive_result(std::unique_ptr<TestResult> &&test_result)
{
}
void TargetTestExecutableBase::distribute_result(Requirement &) const
{
}

Glib::RefPtr<Gtk::StringList> TargetTestExecutableBase::get_fixtures_model() const
{
    return fixtures_model;
}

} // namespace optifol
