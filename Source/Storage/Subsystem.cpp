/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the subsystem-level storage object
 * @author Oliver Dixon
 * @date 2025-02-22
 * @version Development
 */

#include "Subsystem.hpp"

#include "StorageHashFunctor.hpp"
#include "../Logging.hpp"

namespace optifol
{

Subsystem::Subsystem(std::string &&name, TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    TreeNode(parent)
{
    property_name().set_value(std::move(name));
}

Subsystem::Subsystem(std::string &&name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder,
        TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    StorageObjectBase(cobject, builder),
    TreeNode(parent)
{
    property_name().set_value(std::move(name));
}

bool Subsystem::operator==(const Subsystem &other) const noexcept
{
    return std::hash<Subsystem>{}(*this) == std::hash<Subsystem>{}(other);
}

std::string Subsystem::get_path() const
{
    const auto hash = std::hash<Subsystem>{}(*this);
    if (hash != fully_qualified_path_cache.first) {
        fully_qualified_path_cache.first = hash;
        fully_qualified_path_cache.second = get_parent()->get_path() + '/' + property_name().get_value();
    }

    return fully_qualified_path_cache.second;
}

}
