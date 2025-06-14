/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 6/14/25.
//

#include "Function.hpp"

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
    // TODO URGENT compartmentalise into iterator-based static member function
    std::string result = get_disambiguated_name() + '(';

    auto argument_count = arguments.size();

    for (const auto &arg: arguments) {
        result += arg->to_string();
        if (--argument_count > 0)
            result += ", ";
    }

    result += ')';
    return result;
}

std::string Function::get_disambiguated_name() const
{
    return name;
}

std::ostream &Function::serialise(std::ostream &ostream) const
{
    // TODO URGENT compartmentalise into iterator-based static member function
    ostream << '$' << name << '(';

    if (arguments.empty() == false) {
        const auto argument_count = arguments.size() - 1;

        for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx) {
            arguments[argument_idx]->serialise(ostream);
            ostream << ", ";
        }

        arguments[argument_count]->serialise(ostream);
    }

    return ostream << ')';
}

} // namespace optifol
