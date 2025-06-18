/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Function IR node
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
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

std::string Variable::to_string() const
{
    return name;
}

std::string_view Variable::get_disambiguated_name() const
{
    return disambiguated_name.value_or(name);
}

} // namespace optifol
