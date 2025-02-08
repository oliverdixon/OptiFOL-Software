/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file IObservingSentenceVisitor.hpp
 * @brief Interface specification for the sentence-observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#ifndef OPTIFOL_IOBSERVINGSENTENCEVISITOR_HPP
#define OPTIFOL_IOBSERVINGSENTENCEVISITOR_HPP

#include <any>

namespace optifol
{

class QuantifiedSentenceNode;
class ConnectedSentenceNode;
class NegatedSentenceNode;
class NodeProxy;
class IdentitySentenceNode;
class PredicationNode;

class IObservingSentenceVisitor
{
public:
    virtual ~IObservingSentenceVisitor() = default;

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

#endif //OPTIFOL_IOBSERVINGSENTENCEVISITOR_HPP
