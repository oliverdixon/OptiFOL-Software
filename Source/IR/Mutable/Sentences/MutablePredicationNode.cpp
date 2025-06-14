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

#include "MutablePredicationNode.hpp"
#include "../../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../../Visitors/Unification/UnificationVisitor.hpp"
#include "../Terms/IMutableTermNode.hpp"
#include "../Terms/MutableVariableNode.hpp"

namespace optifol
{

MutablePredicationNode::MutablePredicationNode(std::string name, const bool is_positive,
                                 std::vector<std::unique_ptr<IMutableTermNode>> &&arguments) :
    name(std::move(name)), arguments(std::move(arguments)), is_positive(is_positive)
{
}

MutablePredicationNode::MutablePredicationNode(std::string name, const bool is_positive,
                                 const std::vector<std::unique_ptr<IMutableTermNode>> &arguments) :
    name(std::move(name)), is_positive(is_positive)
{
    this->arguments.reserve(arguments.size());
    for (const auto &arg: arguments)
        this->arguments.push_back(arg->clone());
}

MutablePredicationNode::MutablePredicationNode(std::string name, std::vector<std::unique_ptr<IMutableTermNode>> &&arguments) :
    MutablePredicationNode(std::move(name), true, std::move(arguments))
{
}

MutablePredicationNode::MutablePredicationNode(std::string name, const std::vector<std::unique_ptr<IMutableTermNode>> &arguments) :
    MutablePredicationNode(std::move(name), true, arguments)
{
}

MutablePredicationNode::MutablePredicationNode(std::string name) :
    MutablePredicationNode(std::move(name), true, {})
{
}

std::unique_ptr<IMutableSentenceNode> MutablePredicationNode::clone() const
{
    std::vector<std::unique_ptr<IMutableTermNode>> cloned_arguments;
    cloned_arguments.reserve(arguments.size());
    for (const auto& argument : arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<MutablePredicationNode>(name, is_positive, std::move(cloned_arguments));
}

void MutablePredicationNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool MutablePredicationNode::is_negative_polarity() const
{
    return !is_positive;
}

void MutablePredicationNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutablePredicationNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

bool MutablePredicationNode::accept(UnificationVisitor &visitor, const MutablePredicationNode &target) const
{
    return visitor.visit(*this, target);
}

std::size_t MutablePredicationNode::hash() const noexcept
{
    auto hash = std::hash<std::string>{}(name);
    for (const auto &argument: arguments)
        hash = hash_combine(hash, argument->hash());

    return hash_polarity(hash, is_negative_polarity());
}

std::ostream &MutablePredicationNode::serialise(std::ostream &ostream) const
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
