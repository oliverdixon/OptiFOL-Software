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

#include "Function.hpp"

#include "../../CompositeSerialisationHelpers.hpp"

namespace optifol
{

Function::Function(std::string name, const std::initializer_list<const IProcessedTerm *> arguments) : name(std::move(name))
{
    this->arguments.reserve(arguments.size());
    for (const auto argument: arguments)
        this->arguments.push_back(argument);
}

const std::vector<const IProcessedTerm *> &Function::observe_arguments() const noexcept
{
    return arguments;
}

std::string Function::to_string() const
{
    return CompositeSerialisationHelpers::string_serialise(name, arguments.cbegin(), arguments.cend());
}

std::string Function::get_disambiguated_name() const
{
    return name;
}

std::ostream &Function::serialise(std::ostream &ostream) const
{
    return CompositeSerialisationHelpers::stream_serialise(ostream, name, arguments.cbegin(), arguments.cend());
}

std::size_t Function::hash() const noexcept
{
    return composite_hash(name, arguments.cbegin(), arguments.cend());
}

} // namespace optifol
