/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the TestGroup grouping GLib object
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef TESTGROUP_HPP
#define TESTGROUP_HPP

#include "../GUI/ProcessExecutor.hpp"
#include "RequirementGroupBase.hpp"
#include "StorageObjectBase.hpp"

namespace optifol
{

class TestGroup : public StorageObjectBase,
                  public RequirementGroupBase
{
public:
    explicit TestGroup(const Glib::ustring &name);

    TestGroup(const Glib::ustring &name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);
};

} // namespace optifol

#endif // TESTGROUP_HPP
