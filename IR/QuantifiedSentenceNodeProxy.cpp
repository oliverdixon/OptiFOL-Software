//
// Created by owd on 24/11/24.
//

#include "QuantifiedSentenceNodeProxy.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

QuantifiedSentenceNodeProxy::QuantifiedSentenceNodeProxy(std::shared_ptr<QuantifiedSentenceNode> node) :
        sentence(std::move(node))
{}

std::string QuantifiedSentenceNodeProxy::to_string() const
{
    return sentence->to_string();
}

void QuantifiedSentenceNodeProxy::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

}
