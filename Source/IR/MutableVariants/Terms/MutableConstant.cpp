/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableConstant.hpp"

#include "../../../Visitors/MutableTargets/Observers/IObservingNodeVisitor.hpp"
#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Terms/MutatingTermVisitorBase.hpp"
#include "../../Terms/Constant.hpp"

namespace optifol
{

MutableConstant::MutableConstant(std::string name) :
    name(std::move(name))
{
}

std::unique_ptr<IMutableTerm> MutableConstant::clone() const
{
    return std::make_unique<MutableConstant>(name);
}

std::string MutableConstant::to_string() const
{
    return name;
}

std::string_view MutableConstant::get_disambiguated_name() const
{
    return name;
}

void MutableConstant::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

const IProcessedTerm *MutableConstant::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

void MutableConstant::accept(IObservingNodeVisitor &visitor) const
{
    visitor.visit(*this);
}

} // namespace optifol
