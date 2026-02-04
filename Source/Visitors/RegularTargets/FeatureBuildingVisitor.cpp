/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 2/4/26.
//

#include "FeatureBuildingVisitor.hpp"

#include "../../IR/Sentences/Clause.hpp"
#include "../../IR/Terms/Function.hpp"

namespace optifol
{

void FeatureBuildingVisitor::visit(const Clause *const clause) noexcept
{
    reset_counters();

    seen_constants.clear();
    seen_functions.clear();
    seen_variables.clear();

    for (const auto literal : *clause)
        literal->accept(*this);
}

void FeatureBuildingVisitor::visit(const Literal *const literal) noexcept
{
    enter();
    ++features.literal_count;

    const auto& args = literal->observe_arguments();
    for (const auto argument : args)
        argument->accept(*this);

    exit();
}

void FeatureBuildingVisitor::visit(const Constant *const constant) noexcept
{
    std::ignore = *constant;

    // A constant symbol is a 0-arity function symbol.
    enter();

    const auto [it, was_new] = seen_constants.insert(constant);
    if (was_new)
        ++features.function_count;

    exit();
}

void FeatureBuildingVisitor::visit(const Function *const function) noexcept
{
    enter();

    const auto [it, was_new] = seen_functions.insert(function);
    if (was_new) {
        ++features.function_count;
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
        ++features.variable_count;

    exit();
}

std::vector<unsigned int> FeatureBuildingVisitor::extract_sorted_vector()
{
    auto sorted_vector = std::vector{
        features.max_depth,
        features.literal_count,
        features.function_count,
        features.variable_count
    };

    reset_counters();
    return sorted_vector;
}

void FeatureBuildingVisitor::enter() noexcept
{
    features.max_depth = std::max(features.max_depth, ++current_depth);
}

void FeatureBuildingVisitor::exit() noexcept
{
    --current_depth;
}

void FeatureBuildingVisitor::reset_counters() noexcept
{
    current_depth = 0;

    features.max_depth = 0;
    features.literal_count = 0;
    features.function_count = 0;
    features.variable_count = 0;
}

} // namespace optifol
