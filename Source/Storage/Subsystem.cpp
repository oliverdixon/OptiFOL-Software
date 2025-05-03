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

#include <cassert>

#include "Subsystem.hpp"

#include "StorageHashFunctor.hpp"
#include "../Logging.hpp"

namespace optifol
{

Subsystem::Subsystem(std::string &&name, TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    TreeNode(parent)
{
    assert(analysis_groups->get_n_items() == 0);

    property_name().set_value(std::move(name));
    analysis_groups->append(Glib::make_refptr_for_instance(new AnalysisGroup("Unassigned Requirements")));

    requirements->signal_items_changed().connect([this](const guint position, const guint removed, const guint added)
    {
        const auto unassigned_group = analysis_groups->get_item(0);

        if (added == 1)
            unassigned_group->requirements->append(requirements->get_item(position));
        else if (added > 1) {
            std::vector<Glib::RefPtr<Requirement>> additions;
            additions.reserve(added);
            for (guint idx = position; idx < added; ++idx)
                additions.push_back(requirements->get_item(idx));
            unassigned_group->requirements->splice(0, 0, additions);
        }

        // TODO how do we handle removals? From the unassigned group is hard enough, but what about all others?
    });

    assert(analysis_groups->get_n_items() == 1);
}

Subsystem::Subsystem(std::string &&name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder,
        TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    StorageObjectBase(cobject, builder),
    TreeNode(parent)
{
    property_name().set_value(std::move(name));
    analysis_groups->append(Glib::make_refptr_for_instance(new AnalysisGroup("Unassigned Requirements")));
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
