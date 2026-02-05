/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 2/4/26.
//

#ifndef OPTIFOL_FEATUREBUILDINGVISITOR_HPP
#define OPTIFOL_FEATUREBUILDINGVISITOR_HPP

#include <vector>

#include "../../IR/Terms/Constant.hpp"
#include "../../IR/Terms/Function.hpp"
#include "../../IR/Terms/Variable.hpp"
#include "../../Inference/Feature.hpp"
#include "../../Optifol.hpp"

namespace optifol
{

class Clause;
class Literal;
class SkolemFunction;

class FeatureBuildingVisitor
{
public:
    FeatureBuildingVisitor();

    void visit(const Clause *clause) noexcept;

    void visit(const Literal *literal) noexcept;

    void visit(const Constant *constant) noexcept;

    void visit(const Function *function) noexcept;

    void visit(const SkolemFunction *skolem_function) noexcept;

    void visit(const Variable *variable) noexcept;

    std::vector<Feature> extract_sorted_vector();

private:
    void enter() noexcept;

    void exit() noexcept;

    void reset_counters() noexcept;

    unsigned int current_depth = 0;

    std::vector<Feature> features;

    RawUnorderedSet<const Constant> seen_constants;
    RawUnorderedSet<const Function> seen_functions;
    RawUnorderedSet<const Variable> seen_variables;
};

} // namespace optifol

#endif // OPTIFOL_FEATUREBUILDINGVISITOR_HPP
