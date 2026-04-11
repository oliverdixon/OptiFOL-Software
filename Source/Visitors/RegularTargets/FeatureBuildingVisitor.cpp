/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the feature-building visitor
 * @author Oliver Dixon
 * @date 2026-02-04
 * @version Development
 */

#include "FeatureBuildingVisitor.hpp"

#include "../../IR/Sentences/Clause.hpp"
#include "../../IR/Terms/Function.hpp"

namespace optifol
{

FeatureBuildingVisitor::FeatureBuildingVisitor()
{
    features.reserve(Feature::feature_types.size());
    for (const auto feature_type : Feature::feature_types)
        features.emplace_back(feature_type);
}

void FeatureBuildingVisitor::visit(const Clause *const clause) noexcept
{
    reset_counters();

    seen_functions.clear();
    seen_variables.clear();

    for (const auto literal : *clause)
        literal->accept(*this);
}

void FeatureBuildingVisitor::visit(const Literal *const literal) noexcept
{
    enter();
    Feature::get(features, Feature::FeatureType::LiteralCount).increment();

    const auto& args = literal->observe_arguments();
    for (const auto argument : args)
        argument->accept(*this);

    exit();
}

void FeatureBuildingVisitor::visit(const Function *const function) noexcept
{
    enter();

    const auto [it, was_new] = seen_functions.insert(function);
    if (was_new) {
        Feature::get(features, Feature::FeatureType::FunctionCount).increment();
        const auto& args = function->observe_arguments();
        for (const auto argument : args)
            argument->accept(*this);
    }

    exit();
}

void FeatureBuildingVisitor::visit(const SkolemFunction *const skolem_function) noexcept
{
    std::ignore = *skolem_function;

    // Special case: Skolems should not contribute toward the function-count feature, as they are always synthesised.
    enter();
    exit();
}

void FeatureBuildingVisitor::visit(const Variable *const variable) noexcept
{
    enter();

    const auto [it, was_new] = seen_variables.insert(variable);
    if (was_new)
        Feature::get(features, Feature::FeatureType::VariableCount).increment();

    exit();
}

std::vector<Feature> FeatureBuildingVisitor::extract_sorted_vector()
{
    auto saved_features = features;
    reset_counters();
    return saved_features;
}

void FeatureBuildingVisitor::enter() noexcept
{
    Feature::get(features, Feature::FeatureType::MaxDepth).maximise(++current_depth);
}

void FeatureBuildingVisitor::exit() noexcept
{
    --current_depth;
}

void FeatureBuildingVisitor::reset_counters() noexcept
{
    current_depth = 0;

    for (auto& feature : features)
        feature.reset();
}

} // namespace optifol
