/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the feature-building visitor
 * @author Oliver Dixon
 * @date 2026-02-04
 * @version Development
 */

#ifndef OPTIFOL_FEATUREBUILDINGVISITOR_HPP
#define OPTIFOL_FEATUREBUILDINGVISITOR_HPP

#include <vector>

#include "../../IR/Terms/Function.hpp"
#include "../../IR/Terms/Variable.hpp"
#include "../../Inference/Feature.hpp"
#include "../../Optifol.hpp"

namespace optifol
{

class Clause;
class Literal;
class SkolemFunction;

/**
 * @class FeatureBuildingVisitor
 * @brief The FeatureBuildingVisitor recurses over a Clause structure and builds up features according to the state of
 *  the immutable AST.
 */
class FeatureBuildingVisitor
{
public:
    FeatureBuildingVisitor();

    void visit(const Clause *clause) noexcept;

    void visit(const Literal *literal) noexcept;

    void visit(const Function *function) noexcept;

    void visit(const SkolemFunction *skolem_function) noexcept;

    void visit(const Variable *variable) noexcept;

    /**
     * @brief Extract the features computed since the previous invocation on the Clause.
     * @return The stored features, in the order specified by the Feature comparator.
     */
    std::vector<Feature> extract_sorted_vector();

private:
    void enter() noexcept;

    void exit() noexcept;

    void reset_counters() noexcept;

    unsigned int current_depth = 0;

    std::vector<Feature> features;

    RawUnorderedSet<const Function> seen_functions;
    RawUnorderedSet<const Variable> seen_variables;
};

} // namespace optifol

#endif // OPTIFOL_FEATUREBUILDINGVISITOR_HPP
