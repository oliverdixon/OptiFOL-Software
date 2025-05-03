/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#include "PredicationNode.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

PredicationNode::PredicationNode(std::string name, std::vector<std::unique_ptr<ITermNode>> &&arguments) :
        name(std::move(name)),
        arguments(std::move(arguments))
{}

void PredicationNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void PredicationNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

}
