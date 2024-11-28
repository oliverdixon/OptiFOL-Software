//
// Created by owd on 24/11/24.
//

#include "NodeProxy.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

NodeProxy::NodeProxy(std::shared_ptr<ISentenceNode> node) :
        sentence(std::move(node))
{}

std::string NodeProxy::to_string() const
{
    return sentence->to_string();
}

void NodeProxy::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

}
