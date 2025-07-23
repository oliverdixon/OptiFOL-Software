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

    requirements->signal_items_changed().connect([this](const guint position,
        const guint removed_count, const guint added_count)
    {
        const auto default_analysis_group = analysis_groups->get_item(0);
        const auto default_test_group = test_groups->get_item(0);

        if (added_count == 1) {
            const auto candidate = requirements->get_item(position);

            if (candidate->is_analysis_ready())
                default_analysis_group->insert(candidate);

            if (candidate->observe_test().has_value())
                default_test_group->insert(candidate);
        }

        else if (added_count > 1) {
            std::vector<Glib::RefPtr<Requirement>> analysis_additions;
            std::vector<Glib::RefPtr<Requirement>> testing_additions;

            analysis_additions.reserve(added_count);
            testing_additions.reserve(added_count);

            for (guint added_list_idx = position; added_list_idx < added_count; ++added_list_idx) {
                const auto candidate = requirements->get_item(added_list_idx);

                if (candidate->is_analysis_ready())
                    analysis_additions.push_back(candidate);

                if (candidate->observe_test().has_value())
                    testing_additions.push_back(candidate);
            }

            default_analysis_group->insert(analysis_additions);
            default_test_group->insert(testing_additions);
        }

        // TODO how do we handle removals? From the unassigned group is hard enough, but what about all others?
        std::ignore = removed_count;
    });

    assert(analysis_groups->get_n_items() == 1);
    assert(test_groups->get_n_items() == 1);
}

} // namespace optifol
