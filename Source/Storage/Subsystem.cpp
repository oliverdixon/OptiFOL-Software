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
#include "../Logging.hpp"

namespace optifol
{

Subsystem::Subsystem(const Glib::ustring& name, TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    TreeNode(parent)
{
    setup_groups(name);
}

Subsystem::Subsystem(const Glib::ustring& name, BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder,
        TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    StorageObjectBase(cobject, builder),
    TreeNode(parent)
{
    setup_groups(name);
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

void Subsystem::setup_groups(const Glib::ustring &name)
{
    assert(analysis_groups->get_n_items() == 0);

    property_name().set_value(name);
    analysis_groups->append(Glib::make_refptr_for_instance(new AnalysisGroup("Unassigned Requirements")));
    test_groups->append(Glib::make_refptr_for_instance(new TestGroup("Unassigned Requirements")));

    requirements->signal_items_changed().connect([this](const guint position, const guint removed, const guint added)
    {
        const auto default_analysis_group = analysis_groups->get_item(0);
        const auto default_test_group = test_groups->get_item(0);

        if (added == 1) {
            default_analysis_group->requirements->append(requirements->get_item(position));
            default_test_group->requirements->append(requirements->get_item(position));
        }

        else if (added > 1) {
            std::vector<Glib::RefPtr<Requirement>> additions;
            additions.reserve(added);

            for (guint idx = position; idx < added; ++idx)
                additions.push_back(requirements->get_item(idx));

            default_analysis_group->requirements->splice(0, 0, additions);
            default_test_group->requirements->splice(0, 0, additions);
        }

        // TODO how do we handle removals? From the unassigned group is hard enough, but what about all others?
    });

    assert(analysis_groups->get_n_items() == 1);
    assert(test_groups->get_n_items() == 1);
}

} // namespace optifol
