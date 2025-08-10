/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 8/10/25.
//

#include "PartialTestResult.hpp"

namespace optifol
{

PartialTestResult::PartialTestResult(std::string file, const std::size_t line, const std::string &message) :
    Glib::ObjectBase("PartialTestResult"),
    file(*this, "PartialTestResult-file", std::move(file)),
    line(*this, "PartialTestResult-line", static_cast<guint>(line))
{
    property_name().set_value(message);
}

PartialTestResult::PartialTestResult(std::string file, const std::size_t line, const std::string &message,
        BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Glib::ObjectBase("PartialTestResult"),
    StorageObjectBase(cobject, builder),
    file(*this, "PartialTestResult-file", std::move(file)),
    line(*this, "PartialTestResult-line", static_cast<guint>(line))
{
    property_name().set_value(message);
}

Glib::RefPtr<Gtk::TreeListModel> PartialTestResult::get_tests_tree() const noexcept
{
    return nullptr;
}

Glib::RefPtr<Gtk::TreeListModel> PartialTestResult::get_results_tree() const noexcept
{
    return nullptr;
}

Glib::PropertyProxy<Glib::ustring> PartialTestResult::property_file()
{
    return file.get_proxy();
}

Glib::PropertyProxy<guint> PartialTestResult::property_line()
{
    return line.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> PartialTestResult::property_file() const
{
    return file.get_proxy();
}

Glib::PropertyProxy_ReadOnly<guint> PartialTestResult::property_line() const
{
    return line.get_proxy();
}

} // namespace optifol
