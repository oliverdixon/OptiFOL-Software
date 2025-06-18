/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableVariable.hpp"

#include "../../../Visitors/MutableTargets/Terms/MutatingTermVisitorBase.hpp"
#include "MutableFunction.hpp"

namespace optifol
{

MutableVariable::MutableVariable(std::string name) :
    name(std::move(name))
{
}

MutableVariable::MutableVariable(std::string name, const std::string &disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(disambiguated_name)
{
}

std::unique_ptr<IMutableTerm> MutableVariable::clone() const
{
    if (disambiguated_name.has_value())
        return std::make_unique<MutableVariable>(name, *disambiguated_name);

    return std::make_unique<MutableVariable>(name);
}

void MutableVariable::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

std::string MutableVariable::to_string() const
{
    return name;
}

std::string_view MutableVariable::get_disambiguated_name() const
{
    if (disambiguated_name.has_value())
        return *disambiguated_name;

    return name;
}

} // namespace optifol
