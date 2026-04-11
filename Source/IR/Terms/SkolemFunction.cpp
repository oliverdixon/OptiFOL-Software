/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Skolem Function IR node
 * @author Oliver Dixon
 * @date 2025-01-30
 * @version Development
 */

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
