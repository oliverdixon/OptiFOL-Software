/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#include "NodeProxy.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

NodeProxy::NodeProxy(std::unique_ptr<ISentenceNode>&& node) :
        sentence(std::move(node))
{}

void NodeProxy::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void NodeProxy::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

}
