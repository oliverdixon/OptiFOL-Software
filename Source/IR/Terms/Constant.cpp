/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Constant Term IR node
 * @author Oliver Dixon
 * @date 2025-06-18
 * @version Development
 */

#include "Constant.hpp"

#include "../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"

namespace optifol
{

Constant::Constant(std::string name) :
    name(std::move(name))
{
}

std::string Constant::to_string() const
{
    return name;
}

std::string_view Constant::get_disambiguated_name() const
{
    return name;
}

} // namespace optifol
