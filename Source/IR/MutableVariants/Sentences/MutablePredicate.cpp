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

#include "../../../Visitors/MutableTargets/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../Terms/IMutableTerm.hpp"
#include "../Terms/MutableVariable.hpp"
#include "MutablePredicate.hpp"

namespace optifol
{

MutablePredicate::MutablePredicate(std::string name, const bool is_positive,
                                 std::vector<std::unique_ptr<IMutableTerm>> &&arguments) :
    name(std::move(name)), arguments(std::move(arguments)), is_positive(is_positive)
{
}

MutablePredicate::MutablePredicate(std::string name, const bool is_positive,
                                 const std::vector<std::unique_ptr<IMutableTerm>> &arguments) :
    name(std::move(name)), is_positive(is_positive)
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
    for (const auto& argument : arguments)
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

void MutablePredicate::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutablePredicate::hash() const noexcept
{
    auto hash = std::hash<std::string>{}(name);
    for (const auto &argument: arguments)
        hash = hash_combine(hash, argument->hash());

    return hash_polarity(hash, is_negative_polarity());
}

std::ostream &MutablePredicate::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << name << '(';

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

}
