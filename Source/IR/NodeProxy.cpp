//
// Created by owd on 24/11/24.
//

#include "NodeProxy.hpp"
#include "../Visitors/Sentences/MutatingVisitorBase.hpp"
#include "../Visitors/Sentences/IObservingVisitor.hpp"

namespace optifol
{

NodeProxy::NodeProxy(std::shared_ptr<ISentenceNode> node) :
        sentence(std::move(node))
{}

void NodeProxy::accept(MutatingVisitorBase &visitor)
{
    visitor.visit(*this);
}

void NodeProxy::accept(IObservingVisitor &visitor) const
{
    visitor.visit(*this);
}

}
