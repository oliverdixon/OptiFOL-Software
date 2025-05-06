/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Mutating Sentence Visitor base class.
 * @author Oliver Dixon
 * @date 2024-12-01
 * @version Development
 */

#ifndef MUTATINGSENTENCEVISITORBASE_HPP
#define MUTATINGSENTENCEVISITORBASE_HPP

namespace optifol
{

class QuantifiedSentenceNode;
class ConnectedSentenceNode;
class IdentitySentenceNode;
class PredicationNode;
class SentenceRoot;

/**
 * @class MutatingSentenceVisitorBase
 * @brief The Mutating Sentence Visitor Base provides an abstract base for sentence-mutating visitors.
 * @note Visitor implementations may optionally maintain an internal state to track their position within the AST. The
 *  @ref MutatingSentenceVisitorBase::reset() member function will reset any internal state to its implementation-
 *  defined default value.
 */
class MutatingSentenceVisitorBase
{
public:
    virtual ~MutatingSentenceVisitorBase() = default;

    /**
     * @brief Applies a mutating transformation to the given quantified sentence node, including the quantified
     *  sentence.
     * @param node The quantified sentence root node of the transformation target.
     */
    virtual void visit(QuantifiedSentenceNode& node);

    /**
     * @brief Applies a mutating transformation to the given connected sentence node and both operands.
     * @param node The connected sentence root node of the transformation target.
     */
    virtual void visit(ConnectedSentenceNode& node);

    /**
     * @brief Applies a mutating transformation to the given term-identity sentence node and both operands.
     * @param node The identity sentence root node of the transformation target.
     */
    virtual void visit(IdentitySentenceNode& node);

    /**
     * @brief Applies a mutating transformation to the given predicate sentence node and all arguments.
     * @param node The predicate sentence root node of the transformation target.
     */
    virtual void visit(PredicationNode& node);

    /**
     * @brief Applies a mutating transformation to the given sentence root
     * @param node The sentence root container containing the root of the sentence.
     */
    virtual void visit(SentenceRoot& node);

    /**
     * @brief Reset any internal state of the visitor.
     * @note This member function is purely virtual to prevent authors of stateful visitors forgetting to implement the
     *  required resetter.
     */
    virtual void reset() = 0;
};

}

#endif
