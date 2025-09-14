/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Predicate IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutablePredicate.hpp"

#include "../../../CompositeSerialisationHelpers.hpp"
#include "../../../Visitors/MutableTargets/Observers/IObservingNodeVisitor.hpp"
#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Sentences/Literal.hpp"
#include "../Terms/IMutableTerm.hpp"

namespace optifol
{

MutablePredicate::MutablePredicate(
        std::string name, const bool is_positive, std::vector<std::unique_ptr<IMutableTerm>> &&arguments) :
    name(std::move(name)),
    arguments(std::move(arguments)),
    is_positive(is_positive)
{
}

MutablePredicate::MutablePredicate(
        std::string name, const bool is_positive, const std::vector<std::unique_ptr<IMutableTerm>> &arguments) :
    name(std::move(name)),
    is_positive(is_positive)
{
    this->arguments.reserve(arguments.size());
    for (const auto &arg: arguments)
        this->arguments.push_back(arg->clone());
}

MutablePredicate::MutablePredicate(std::string name, std::vector<std::unique_ptr<IMutableTerm>> &&arguments) :
    MutablePredicate(std::move(name), true, std::move(arguments))
{
}

MutablePredicate::MutablePredicate(std::string name, const std::vector<std::unique_ptr<IMutableTerm>> &arguments) :
    MutablePredicate(std::move(name), true, arguments)
{
}

MutablePredicate::MutablePredicate(std::string name) :
    MutablePredicate(std::move(name), true, {})
{
}

std::unique_ptr<IMutableSentence> MutablePredicate::clone() const
{
    std::vector<std::unique_ptr<IMutableTerm>> cloned_arguments;
    cloned_arguments.reserve(arguments.size());
    for (const auto &argument: arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<MutablePredicate>(name, is_positive, std::move(cloned_arguments));
}

void MutablePredicate::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool MutablePredicate::is_negative_polarity() const noexcept
{
    return !is_positive;
}

void MutablePredicate::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutablePredicate::accept(IObservingNodeVisitor &visitor) const
{
    visitor.visit(*this);
}

const IProcessedSentence *MutablePredicate::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

std::size_t MutablePredicate::hash() const noexcept
{
    return composite_hash(name, arguments.cbegin(), arguments.cend(), is_negative_polarity());
}

std::ostream &MutablePredicate::serialise(std::ostream &ostream) const
{
    return CompositeSerialisationHelpers::stream_serialise(
            ostream, name, arguments.cbegin(), arguments.cend(), is_negative_polarity());
}

bool MutablePredicate::operator==(const IMutableSentence &other) const noexcept
{
    const auto other_predicate = dynamic_cast<const MutablePredicate *>(&other);
    if (other_predicate == nullptr)
        // Other IMutableSentence isn't a MutablePredicate.
        return false;

    if (name != other_predicate->name)
        // Different superficial names.
        return false;

    const auto argument_count = arguments.size();
    if (argument_count != other_predicate->arguments.size())
        // Different number of arguments.
        return false;

    for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx) {
        const auto& lhs_arg_ptr = arguments[argument_idx];
        const auto& rhs_arg_ptr = other_predicate->arguments[argument_idx];

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

std::string_view MutablePredicate::get_name() const noexcept
{
    return name;
}

const std::vector<std::unique_ptr<IMutableTerm>> &MutablePredicate::observe_arguments() const noexcept
{
    return arguments;
}

std::vector<std::unique_ptr<IMutableTerm>> &MutablePredicate::observe_arguments() noexcept
{
    return arguments;
}

} // namespace optifol
