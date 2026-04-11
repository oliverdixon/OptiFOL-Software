/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for an analysis grouping of requirements
 * @author Oliver Dixon
 * @date 2025-04-27
 * @version Development
 */

#ifndef ANALYSISGROUP_HPP
#define ANALYSISGROUP_HPP

#include "../Inference/Prover.hpp"
#include "ObjectGroup.hpp"
#include "Requirement.hpp"

namespace optifol
{

class AnalysisGroup : public StorageObjectBase,
                      public ObjectGroup<Requirement>
{
public:
    /**
     * @brief Create a new Analysis Group with the given name and register in the Glib GType system
     * @param name The initial name of the Analysis Group
     * @param symbol_repository The associated SymbolRepository for Clause objects used by Requirement nodes
     * in the AnalysisGroup.
     */
    explicit AnalysisGroup(const Glib::ustring &name, std::shared_ptr<SymbolRepository> symbol_repository);

    AnalysisGroup(const Glib::ustring &name, BaseObjectType *cobject,
            const Glib::RefPtr<Gtk::Builder> &builder, std::shared_ptr<SymbolRepository> symbol_repository);

    Prover &observe_prover_instance() noexcept;

private:
    void handle_group_model_change(guint initial_index, guint removed_count, guint added_count);

    Prover kb;
};

} // namespace optifol

#endif
