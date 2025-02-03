/*
 * Copyright (c) All Rights Reserved
 * 2024-2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 24/11/24.
//

#include "PredicationNode.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

PredicationNode::PredicationNode(std::string name, std::vector<std::shared_ptr<ITermNode>> &&arguments) :
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
