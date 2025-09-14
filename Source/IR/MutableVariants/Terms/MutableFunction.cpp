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
#include "../../../Visitors/MutableTargets/Observers/IObservingNodeVisitor.hpp"
#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Terms/MutatingTermVisitorBase.hpp"
#include "../../Terms/Function.hpp"

namespace optifol
{

MutableFunction::MutableFunction(std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&arguments) :
    name(std::move(name)),
    arguments(std::move(arguments))
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

std::string_view MutableFunction::get_disambiguated_name() const
{
    return name;
}

void MutableFunction::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

const IProcessedTerm *MutableFunction::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

void MutableFunction::accept(IObservingNodeVisitor &visitor) const
{
    visitor.visit(*this);
}

bool MutableFunction::operator==(const IMutableTerm &other) const noexcept
{
    const auto other_function = dynamic_cast<const MutableFunction *>(&other);
    if (other_function == nullptr)
        // Other IMutableTerm isn't a MutableFunction.
        return false;

    if (get_disambiguated_name() != other_function->get_disambiguated_name())
        // Different superficial names.
        return false;

    const auto argument_count = arguments.size();
    if (argument_count != other_function->arguments.size())
        // Different number of arguments.
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx) {
        const auto &lhs_arg_ptr = arguments[argument_idx];
        const auto &rhs_arg_ptr = other_function->arguments[argument_idx];

        if (lhs_arg_ptr == nullptr) {
            if (rhs_arg_ptr != nullptr)
                return false;
        } else if (rhs_arg_ptr == nullptr)
            return false;
        else if (*lhs_arg_ptr != *rhs_arg_ptr)
            return false;
    }

    return true;
}

const std::vector<std::unique_ptr<IMutableTerm>> &MutableFunction::observe_arguments() const noexcept
{
    return arguments;
}

std::vector<std::unique_ptr<IMutableTerm>> &MutableFunction::observe_arguments() noexcept
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
