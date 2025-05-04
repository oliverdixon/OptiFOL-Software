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

std::unique_ptr<ISentenceNode> NodeProxy::take_sentence()
{
    return std::move(sentence);
}

const ISentenceNode * NodeProxy::observe_sentence() const
{
    return sentence.get();
}

void NodeProxy::put_sentence(std::unique_ptr<ISentenceNode> &&sentence)
{
    this->sentence = std::move(sentence);
}

std::unique_ptr<ISentenceNode> NodeProxy::clone() const
{
    return std::make_unique<NodeProxy>(sentence->clone());
}

void NodeProxy::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void NodeProxy::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

}
