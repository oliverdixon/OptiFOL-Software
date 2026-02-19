/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Interface specification for the node-observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#ifndef IOBSERVINGNODEVISITOR_HPP
#define IOBSERVINGNODEVISITOR_HPP

namespace optifol
{

class MutableQuantified;
class MutableBinaryConnected;
class MutablePredicate;
class MutableSentenceRoot;

class MutableFunction;
class MutableSkolemFunction;
class MutableVariable;

class IObservingNodeVisitor
{
public:
    virtual ~IObservingNodeVisitor() = default;

    virtual void visit(const MutableQuantified& node) = 0;

    virtual void visit(const MutableBinaryConnected& node) = 0;

    virtual void visit(const MutablePredicate& node) = 0;

    virtual void visit(const MutableSentenceRoot& node) = 0;

    virtual void visit(const MutableFunction& node) = 0;

    virtual void visit(const MutableVariable& node) = 0;
};

}

#endif
