//
// Created by owd on 24/11/24.
//

#include "ConnectedSentenceNodeProxy.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

ConnectedSentenceNodeProxy::ConnectedSentenceNodeProxy(std::shared_ptr<ConnectedSentenceNode> node) :
        sentence(std::move(node))
{}

std::string ConnectedSentenceNodeProxy::to_string() const
{
    return sentence->to_string();
}

void ConnectedSentenceNodeProxy::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

}
