/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Predicate IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "PredicationNode.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../Terms/ITermNode.hpp"
#include "../Terms/VariableNode.hpp"

namespace optifol
{

PredicationNode::PredicationNode(std::string name, std::vector<std::unique_ptr<ITermNode>> &&arguments,
        const bool is_positive) :
    name(std::move(name)),
    arguments(std::move(arguments)),
    is_positive(is_positive)
{
}
PredicationNode::PredicationNode(std::string name, const bool is_positive) :
    name(std::move(name)),
    is_positive(is_positive)
{
}

std::unique_ptr<ISentenceNode> PredicationNode::clone() const
{
    std::vector<std::unique_ptr<ITermNode>> cloned_arguments;
    cloned_arguments.reserve(arguments.size());
    for (const auto& argument : arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<PredicationNode>(name, std::move(cloned_arguments), is_positive);
}

void PredicationNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool PredicationNode::is_negative_polarity() const
{
    return !is_positive;
}

void PredicationNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void PredicationNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

bool PredicationNode::accept(UnificationVisitor &visitor, const PredicationNode &target) const
{
    return visitor.visit(*this, target);
}

std::size_t PredicationNode::hash() const noexcept
{
    auto hash = std::hash<std::string>{}(name);
    for (const auto &argument: arguments)
        hash = hash_combine(hash, argument->hash());

    return hash_polarity(hash, is_negative_polarity());
}

std::ostream &PredicationNode::serialise(std::ostream &ostream) const
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
