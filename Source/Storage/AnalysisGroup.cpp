/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for an analysis grouping of requirements
 * @author Oliver Dixon
 * @date 2025-04-27
 * @version Development
 */

#include "AnalysisGroup.hpp"

namespace optifol
{

AnalysisGroup::AnalysisGroup(std::string &&name) :
    Glib::ObjectBase("AnalysisGroup")
{
    property_name().set_value(std::move(name));
}

AnalysisGroup::AnalysisGroup(std::string &&name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("AnalysisGroup"),
    StorageObjectBase(cobject, builder)
{
    property_name().set_value(std::move(name));
}

}
