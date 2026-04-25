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

#include <assert.h>

namespace optifol
{

AnalysisGroup::AnalysisGroup(const Glib::ustring &name, std::shared_ptr<SymbolRepository> symbol_repository) :
    Glib::ObjectBase("AnalysisGroup"),
    kb(std::move(symbol_repository)),
    ObjectGroup(sigc::mem_fun(*this, &AnalysisGroup::handle_group_model_change))
{
    property_name().set_value(name);
}

AnalysisGroup::AnalysisGroup(const Glib::ustring &name, BaseObjectType *cobject,
        const Glib::RefPtr<Gtk::Builder> &builder, std::shared_ptr<SymbolRepository> symbol_repository) :
    Glib::ObjectBase("AnalysisGroup"),
    StorageObjectBase(cobject, builder),
    ObjectGroup(sigc::mem_fun(*this, &AnalysisGroup::handle_group_model_change)),
    kb(std::move(symbol_repository))
{
    property_name().set_value(name);
}

Prover &AnalysisGroup::observe_prover_instance() noexcept
{
    return kb;
}

void AnalysisGroup::handle_group_model_change(
        const guint initial_index, const guint, const guint added_count)
{
    for (guint added_list_i = initial_index; added_list_i < initial_index + added_count; ++added_list_i) {
        const auto requirement = get_object_by_index(added_list_i);
        kb.tell(requirement->observe_prepared_sentence());
    }
}

} // namespace optifol
