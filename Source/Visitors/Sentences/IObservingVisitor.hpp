//
// Created by owd on 29/11/24.
//

#ifndef OPTIFOL_IOBSERVINGVISITOR_HPP
#define OPTIFOL_IOBSERVINGVISITOR_HPP

#include <any>

#include "../../IR/NodeProxy.hpp"
#include "../../IR/IdentitySentenceNode.hpp"
#include "../../IR/PredicationNode.hpp"
#include "../../IR/NegatedSentenceNode.hpp"
#include "../../IR/ConnectedSentenceNode.hpp"

namespace optifol
{

class IObservingVisitor
{
public:
    virtual ~IObservingVisitor() = default;

    virtual void visit(const QuantifiedSentenceNode& node) = 0;

    virtual void visit(const ConnectedSentenceNode& node) = 0;

    virtual void visit(const NegatedSentenceNode& node) = 0;

    virtual void visit(const NodeProxy& node) = 0;

    virtual void visit(const IdentitySentenceNode& node) = 0;

    virtual void visit(const PredicationNode& node) = 0;

    [[nodiscard]] virtual std::any extract() const = 0;

    virtual void reset() = 0;
};

}

#endif //OPTIFOL_IOBSERVINGVISITOR_HPP
