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
    return hash_combine(std::hash<std::string>{}(property_name().get_value()),
                        std::hash<std::chrono::system_clock::time_point>{}(property_creation_time().get_value()));
}

StorageObjectBase::StorageObjectBase() :
    Glib::ObjectBase("StorageObjectBase"), name(*this, "StorageObjectBase-name"),
    creation_time(*this, "StorageObjectBase-creation-time", std::chrono::system_clock::now()),
    modified_time(*this, "StorageObjectBase-modified-time", std::chrono::system_clock::now())
{
}

StorageObjectBase::StorageObjectBase(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &) :
    Glib::ObjectBase("StorageObjectBase"), Glib::Object(cobject), name(*this, "StorageObjectBase-name"),
    creation_time(*this, "StorageObjectBase-creation-time", std::chrono::system_clock::now()),
    modified_time(*this, "StorageObjectBase-modified-time", std::chrono::system_clock::now())
{
}

}
