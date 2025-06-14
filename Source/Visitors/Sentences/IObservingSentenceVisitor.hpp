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

class MutableQuantifiedSentenceNode;
class MutableConnectedSentenceNode;
class MutableIdentitySentenceNode;
class MutablePredicationNode;
class MutableSentenceRoot;

class IObservingSentenceVisitor
{
public:
    virtual ~IObservingSentenceVisitor() = default;

    virtual void visit(const MutableQuantifiedSentenceNode& node) = 0;

    virtual void visit(const MutableConnectedSentenceNode& node) = 0;

    virtual void visit(const MutableIdentitySentenceNode& node) = 0;

    virtual void visit(const MutablePredicationNode& node) = 0;

    virtual void visit(const MutableSentenceRoot& node) = 0;
};

}

#endif
