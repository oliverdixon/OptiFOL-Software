//
// Created by owd on 24/11/24.
//

#include "PredicationNode.hpp"
#include "../Visitors/MutatingVisitorBase.hpp"
#include "../Visitors/IObservingVisitor.hpp"

namespace optifol
{

PredicationNode::PredicationNode(std::string name, std::vector<std::shared_ptr<ITermNode>> &&arguments) :
        name(std::move(name)),
        arguments(std::move(arguments))
{}

void PredicationNode::accept(MutatingVisitorBase &visitor)
{
    visitor.visit(*this);
}

void PredicationNode::accept(IObservingVisitor &visitor) const
{
    visitor.visit(*this);
}

}
