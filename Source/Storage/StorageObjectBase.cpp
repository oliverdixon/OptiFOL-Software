/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the base class of a Storable Object
 * @author Oliver Dixon
 * @date 2025-04-26
 * @version Development
 */

#include <glibmm/binding.h>
#include <gtkmm/label.h>
#include <gtkmm/listitem.h>
#include <gtkmm/treeexpander.h>
#include <gtkmm/treelistmodel.h>

#include "StorageObjectBase.hpp"

#include "../LegacyWrappers.hpp"

namespace optifol
{

Glib::PropertyProxy<Glib::ustring> StorageObjectBase::property_name()
{
    return name.get_proxy();
}

Glib::PropertyProxy<StorageObjectBase::TimeT> StorageObjectBase::property_creation_time()
{
    return creation_time.get_proxy();
}

Glib::PropertyProxy<StorageObjectBase::TimeT> StorageObjectBase::property_modified_time()
{
    return modified_time.get_proxy();
}

Glib::PropertyProxy_ReadOnly<Glib::ustring> StorageObjectBase::property_name() const
{
    return name.get_proxy();
}

Glib::PropertyProxy_ReadOnly<StorageObjectBase::TimeT> StorageObjectBase::property_creation_time() const
{
    return creation_time.get_proxy();
}

Glib::PropertyProxy_ReadOnly<StorageObjectBase::TimeT> StorageObjectBase::property_modified_time() const
{
    return modified_time.get_proxy();
}

std::size_t StorageObjectBase::hash() const noexcept
{
    return std::hash<std::string>{}(property_name().get_value());
}

void StorageObjectBase::bind_name(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(item->property_name(), label->property_label(), Glib::Binding::Flags::SYNC_CREATE);
}

void StorageObjectBase::bind_creation_time(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(item->property_creation_time(), label->property_label(),
                Glib::Binding::Flags::SYNC_CREATE,
                [](const std::chrono::system_clock::time_point &time) { return std::format("{:%c}", time); });
}

void StorageObjectBase::bind_modification_time(const Glib::RefPtr<Gtk::ListItem> &list_item)
{
    const auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    const auto item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->get_item());

    if (label != nullptr && item != nullptr)
        Glib::Binding::bind_property(item->property_modified_time(), label->property_label(),
                Glib::Binding::Flags::SYNC_CREATE,
                [](const std::chrono::system_clock::time_point &time) { return std::format("{:%c}", time); });
}

void StorageObjectBase::bind_name_property_expandable(
        const Glib::RefPtr<Gtk::ListItem> &list_item, const Glib::RefPtr<Gtk::TreeListModel> &tree_model)
{
    const auto position = list_item->get_position();
    const auto model_item = std::dynamic_pointer_cast<StorageObjectBase>(list_item->get_item());
    const auto expander = dynamic_cast<Gtk::TreeExpander *>(list_item->get_child());

    if (position == GTK_INVALID_LIST_POSITION || model_item == nullptr || expander == nullptr)
        return;

    const auto gui_row = tree_model->get_row(position);
    if (!gui_row)
        return;

    expander->set_list_row(gui_row);

    const auto label = dynamic_cast<Gtk::Label *>(expander->get_child());
    if (!label)
        return;

    Glib::Binding::bind_property(
            model_item->property_name(), label->property_label(), Glib::Binding::Flags::SYNC_CREATE);
}

StorageObjectBase::StorageObjectBase() :
    Glib::ObjectBase("StorageObjectBase"),
    name(*this, "StorageObjectBase-name"),
    creation_time(*this, "StorageObjectBase-creation-time", std::chrono::system_clock::now()),
    modified_time(*this, "StorageObjectBase-modified-time", std::chrono::system_clock::now())
{
}

StorageObjectBase::StorageObjectBase(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &) :
    Glib::ObjectBase("StorageObjectBase"),
    Glib::Object(cobject),
    name(*this, "StorageObjectBase-name"),
    creation_time(*this, "StorageObjectBase-creation-time", std::chrono::system_clock::now()),
    modified_time(*this, "StorageObjectBase-modified-time", std::chrono::system_clock::now())
{
}

} // namespace optifol
