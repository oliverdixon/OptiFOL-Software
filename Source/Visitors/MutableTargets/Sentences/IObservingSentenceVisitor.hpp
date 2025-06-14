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

class MutableQuantified;
class MutableBinaryConnected;
class MutableIdentity;
class MutablePredicate;
class MutableSentenceRoot;

class IObservingSentenceVisitor
{
public:
    virtual ~IObservingSentenceVisitor() = default;

    virtual void visit(const MutableQuantified& node) = 0;

    virtual void visit(const MutableBinaryConnected& node) = 0;

    virtual void visit(const MutableIdentity& node) = 0;

    virtual void visit(const MutablePredicate& node) = 0;

    virtual void visit(const MutableSentenceRoot& node) = 0;
};

}

#endif
