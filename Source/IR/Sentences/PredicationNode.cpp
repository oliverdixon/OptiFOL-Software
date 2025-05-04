/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#include "PredicationNode.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../Terms/ITermNode.hpp"

namespace optifol
{

PredicationNode::PredicationNode(std::string name, std::vector<std::unique_ptr<ITermNode>> &&arguments) :
        name(std::move(name)),
        arguments(std::move(arguments))
{}

std::unique_ptr<ISentenceNode> PredicationNode::clone() const
{
    std::vector<std::unique_ptr<ITermNode>> cloned_arguments;
    cloned_arguments.reserve(arguments.size());
    for (const auto& argument : arguments)
        cloned_arguments.push_back(argument->clone());

    return std::make_unique<PredicationNode>(name, std::move(cloned_arguments));
}

void PredicationNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void PredicationNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

}
