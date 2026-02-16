/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Skolem Function Term IR node
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#include "MutableSkolemFunction.hpp"

#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../Terms/SkolemFunction.hpp"

namespace optifol
{

MutableSkolemFunction::MutableSkolemFunction(
        std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&quantified_variables) :
    MutableFunction(std::move(name), std::move(quantified_variables))
{
}

std::unique_ptr<IMutableTerm> MutableSkolemFunction::clone() const
{
    std::vector<std::unique_ptr<IMutableTerm>> cloned_arguments;

    cloned_arguments.reserve(arguments.size());
    for (const auto &argument: arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<MutableSkolemFunction>(name, std::move(cloned_arguments));
}

const IProcessedTerm *MutableSkolemFunction::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

} // namespace optifol
