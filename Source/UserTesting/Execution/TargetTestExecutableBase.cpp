/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the abstract TargetTestExecutableBase
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#include "TargetTestExecutableBase.hpp"

namespace optifol
{

TargetTestExecutableBase::TargetTestExecutableBase(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    StorageObjectBase(cobject, builder)
{
}

} // namespace optifol
