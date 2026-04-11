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

#include "../Logging.hpp"
#include "Subsystem.hpp"

namespace optifol
{

const log4cxx::LoggerPtr Subsystem::subsystem_logger =
        Logging::get_logger({"GUI", "StorageControl", "Subsystem"});

Subsystem::Subsystem(const Glib::ustring &name, TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    TreeNode(parent),
    ObjectGroup(sigc::mem_fun(*this, &Subsystem::handle_requirement_model_change))
{
    setup_groups(name);
}

Subsystem::Subsystem(const Glib::ustring &name, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder, TreeNode *parent) :
    Glib::ObjectBase("Subsystem"),
    StorageObjectBase(cobject, builder),
    TreeNode(parent),
    ObjectGroup(sigc::mem_fun(*this, &Subsystem::handle_requirement_model_change))
{
    setup_groups(name);
}

bool Subsystem::operator==(const Subsystem &other) const noexcept
{
    return hash() == other.hash();
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

void Subsystem::duplicate_requirement(const Requirement &requirement)
{
    build_requirement(requirement.property_name().get_value(), requirement.property_statement().get_value(),
            requirement.property_description().get_value(), requirement.property_priority().get_value(),
            requirement.get_test_specs());
}

Glib::RefPtr<Gio::ListStore<AnalysisGroup>> Subsystem::get_analysis_groups() const noexcept
{
    return analysis_groups;
}

Glib::RefPtr<Gio::ListStore<TestGroup>> Subsystem::get_test_groups() const noexcept
{
    return test_groups;
}

std::shared_ptr<SymbolRepository> Subsystem::share_symbol_repository()
{
    return symbol_repository;
}

void Subsystem::setup_groups(const Glib::ustring &name)
{
    assert(analysis_groups->get_n_items() == 0);
    assert(test_groups->get_n_items() == 0);

    property_name().set_value(name);

    analysis_groups->append(
            Glib::make_refptr_for_instance(new AnalysisGroup("Unassigned Requirements", symbol_repository)));

    test_groups->append(Glib::make_refptr_for_instance(new TestGroup("Unassigned Requirements")));

    assert(analysis_groups->get_n_items() == 1);
    assert(test_groups->get_n_items() == 1);
}

void Subsystem::handle_requirement_model_change(
        const guint initial_index, const guint removed_count, const guint added_count) noexcept
{
    subsystem_logger->debug("Handling requirements change: " + std::to_string(added_count) +
            " additions and " + std::to_string(removed_count) + " deletions at position " +
            std::to_string(initial_index) + '.');

    handle_requirement_deletions(initial_index, removed_count);
    handle_requirement_additions(initial_index, added_count);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive. Called from 'handle_object_change'.
void Subsystem::handle_requirement_deletions(const guint initial_index, const guint removed_count) noexcept
{
    /*
     * First handle removed requirements, making use of the deleted requirements records. Notice that this
     * doesn't use the model at all, as the Requirements have already been erased and moved into the deleted
     * records map.
     */
    // TODO: efficiently remove multiple records with splice. See insertion logic.

    for (guint remove_count_i = 0; remove_count_i < removed_count; ++remove_count_i) {
        try {
            const auto deleted_item = steal_deleted_object(initial_index + remove_count_i);

            // Remove from analysis groups.
            if (deleted_item->is_analysis_ready()) {
                const auto analysis_group_count = analysis_groups->get_n_items();

                for (guint analysis_group_index = 0; analysis_group_index < analysis_group_count;
                        ++analysis_group_index) {
                    const auto &analysis_group = analysis_groups->get_item(analysis_group_index);
                    subsystem_logger->debug("Propagating deletion of \"" +
                            deleted_item->property_name().get_value() + "\" to Analysis Group \"" +
                            analysis_group->property_name().get_value() + "\".");
                    analysis_group->delete_object(deleted_item);
                }
            }

            // Remove from test groups.
            if (deleted_item->has_tests()) {
                const auto test_group_count = test_groups->get_n_items();

                for (guint test_group_index = 0; test_group_index < test_group_count; ++test_group_index) {
                    const auto &test_group = test_groups->get_item(test_group_index);
                    subsystem_logger->debug("Propagating deletion of \"" +
                            deleted_item->property_name().get_value() + "\" to Test Group \"" +
                            test_group->property_name().get_value() + "\".");
                    test_group->delete_object(deleted_item);
                }
            }
        } catch (const std::runtime_error &error) {
            subsystem_logger->error("Could not propagate deletion of Requirement previously at index " +
                    std::to_string(initial_index + remove_count_i) + " through Subsystem \"" +
                    property_name().get_value() + "\".");
            subsystem_logger->error(error.what());
        }
    }
}

// ReSharper disable once CppDFAUnreachableFunctionCall - False positive. Called from 'handle_object_change'.
void Subsystem::handle_requirement_additions(
        const guint initial_index, const guint added_count) const noexcept
{
    const auto default_analysis_group = analysis_groups->get_item(0);
    const auto default_test_group = test_groups->get_item(0);

    if (added_count == 1) {
        try {
            const auto candidate = get_object_by_index(initial_index);

            // Distribute to default analysis group, if it has an OK formula.
            if (candidate->is_analysis_ready()) {
                subsystem_logger->debug("Propagating addition of \"" +
                        candidate->property_name().get_value() + "\" to default Analysis Group.");
                default_analysis_group->insert_object(candidate);
            }

            // Distribute to default test group, if it has at least one test.
            if (candidate->has_tests()) {
                subsystem_logger->debug("Propagating addition of \"" +
                        candidate->property_name().get_value() + "\" to default Test Group.");
                default_test_group->insert_object(candidate);
            }
        } catch (const std::runtime_error &error) {
            subsystem_logger->error("Could not propagate addition of Requirement claimed to be at index " +
                    std::to_string(initial_index) + " through Subsystem \"" + property_name().get_value() +
                    "\".");
            subsystem_logger->error(error.what());
        }
    }

    else if (added_count > 1) {
        // As above, but specialised for multiple insertion using Gtk's splice capability.

        try {
            std::vector<Glib::RefPtr<Requirement>> analysis_additions;
            std::vector<Glib::RefPtr<Requirement>> testing_additions;

            analysis_additions.reserve(added_count);
            testing_additions.reserve(added_count);

            for (guint added_list_i = initial_index; added_list_i < added_count; ++added_list_i) {
                try {
                    const auto candidate = get_object_by_index(added_list_i + initial_index);

                    if (candidate->is_analysis_ready()) {
                        subsystem_logger->debug("Propagating addition of \"" +
                                candidate->property_name().get_value() + "\" to default Analysis Group.");
                        analysis_additions.push_back(candidate);
                    }

                    if (candidate->has_tests()) {
                        subsystem_logger->debug("Propagating addition of \"" +
                                candidate->property_name().get_value() + "\" to default Test Group.");
                        testing_additions.push_back(candidate);
                    }
                } catch (const std::runtime_error &error) {
                    subsystem_logger->error(
                            "Could not propagate addition of Requirement claimed to be at index " +
                            std::to_string(initial_index) + " through Subsystem \"" +
                            property_name().get_value() + "\".");
                    subsystem_logger->error(error.what());
                }
            }

            default_analysis_group->insert_object(analysis_additions);
            default_test_group->insert_object(testing_additions);
        } catch (const std::exception &global_error) {
            subsystem_logger->error("Could not propagate any additions of " + std::to_string(added_count) +
                    " from index " + std::to_string(initial_index) + " for Subsystem " +
                    property_name().get_value() + "\".");
            subsystem_logger->error(global_error.what());
        }
    }
}

} // namespace optifol
