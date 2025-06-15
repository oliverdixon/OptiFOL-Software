/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Function IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableFunction.hpp"

#include "../../../CompositeSerialisationHelpers.hpp"
#include "../../../Visitors/MutableTargets/Terms/MutatingTermVisitorBase.hpp"
#include "../../Terms/Function.hpp"

namespace optifol
{

MutableFunction::MutableFunction(std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&arguments) :
    name(std::move(name)), arguments(std::move(arguments))
{
}

MutableFunction::MutableFunction(std::string name, const std::vector<std::unique_ptr<IMutableTerm>> &arguments) :
    name(std::move(name))
{
    this->arguments.reserve(arguments.size());
    for (const auto &argument: arguments)
        this->arguments.push_back(argument->clone());
}

std::string MutableFunction::to_string() const
{
    return CompositeSerialisationHelpers::string_serialise(name, arguments.cbegin(), arguments.cend());
}

std::unique_ptr<IMutableTerm> MutableFunction::clone() const
{
    std::vector<std::unique_ptr<IMutableTerm>> cloned_arguments;
    cloned_arguments.reserve(arguments.size());
    for (const auto &argument: arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<MutableFunction>(name, std::move(cloned_arguments));
}

std::string MutableFunction::get_disambiguated_name() const
{
    return name;
}

void MutableFunction::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

const std::vector<std::unique_ptr<IMutableTerm>> &MutableFunction::observe_arguments() const
{
    return arguments;
}

std::vector<std::unique_ptr<IMutableTerm>> &MutableFunction::observe_arguments()
{
    return arguments;
}

std::ostream &MutableFunction::serialise(std::ostream &ostream) const
{
    return CompositeSerialisationHelpers::stream_serialise(ostream, name, arguments.cbegin(), arguments.cend());
}

std::size_t MutableFunction::hash() const noexcept
{
    return composite_hash(name, arguments.cbegin(), arguments.cend());
}

} // namespace optifol
