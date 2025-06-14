/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */


#include "Variable.hpp"

namespace optifol
{

Variable::Variable(std::string name) :
    name(std::move(name))
{
}

Variable::Variable(std::string name, const std::string &disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(disambiguated_name)
{
}

std::ostream &Variable::serialise(std::ostream &ostream) const
{
    return ostream << name;
}

std::string Variable::to_string() const
{
    return name;
}

std::string Variable::get_disambiguated_name() const
{
    return disambiguated_name.value_or(name);
}

} // namespace optifol
