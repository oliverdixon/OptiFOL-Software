/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Interface specification for the sentence-observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#ifndef IOBSERVINGSENTENCEVISITOR_HPP
#define IOBSERVINGSENTENCEVISITOR_HPP

namespace optifol
{

class QuantifiedSentenceNode;
class ConnectedSentenceNode;
class IdentitySentenceNode;
class PredicationNode;
class SentenceRoot;

class IObservingSentenceVisitor
{
public:
    virtual ~IObservingSentenceVisitor() = default;

    virtual void visit(const QuantifiedSentenceNode& node) = 0;

    virtual void visit(const ConnectedSentenceNode& node) = 0;

    virtual void visit(const IdentitySentenceNode& node) = 0;

    virtual void visit(const PredicationNode& node) = 0;

    virtual void visit(const SentenceRoot& node) = 0;
};

}

#endif
