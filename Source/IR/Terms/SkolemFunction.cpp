/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#include "SkolemFunction.hpp"

#include "../../Visitors/RegularTargets/FeatureBuildingVisitor.hpp"

namespace optifol
{

SkolemFunction::SkolemFunction(std::string name, std::initializer_list<const IProcessedTerm *> arguments) :
    Function(std::move(name), arguments)
{
}

SkolemFunction::SkolemFunction(std::string name, std::vector<const IProcessedTerm *> &&arguments) :
    Function(std::move(name), std::move(arguments))
{
}

void SkolemFunction::accept(FeatureBuildingVisitor &feature_building_visitor) const noexcept
{
    feature_building_visitor.visit(this);
}

} // namespace optifol
