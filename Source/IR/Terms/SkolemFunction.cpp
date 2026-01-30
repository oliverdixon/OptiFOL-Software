/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#include "SkolemFunction.hpp"

#include "../../Inference/FVI/FeatureComponentBuilder.hpp"

namespace optifol
{

SkolemFunction::SkolemFunction(std::string name, std::initializer_list<const IProcessedTerm *> arguments) :
    Function(std::move(name), std::move(arguments))
{
}

SkolemFunction::SkolemFunction(std::string name, std::vector<const IProcessedTerm *> &&arguments) :
    Function(std::move(name), std::move(arguments))
{
}

void SkolemFunction::accept(const FeatureComponentBuilder &feature_builder) const
{
    feature_builder.visit(this);
}

} // namespace optifol
