/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Disjunction-Distribution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-25
 * @version Development
 */

#ifndef OPTIFOL_DISJUNCTIONDISTRIBUTIONVISITOR_HPP
#define OPTIFOL_DISJUNCTIONDISTRIBUTIONVISITOR_HPP

#include <memory>
#include <stack>

#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class ISentenceNode;

/**
 * @class DisjunctionDistributionVisitor
 * @brief The Disjunction-Distribution Visitor applies the distributive law to eligible connected sentences.
 *
 * @details The Disjunction-Distribution Visitor rewrites the model to distribute disjunctions across clauses of nested
 * conjunctions. In particular,
 * <ul>
 *   <li><code>P | (Q & R)</code> becomes <code>(P | Q) & (P | R)</code>; and likewise, </li>
 *   <li><code>(P & Q) | R</code> becomes <code>(R | P) & (R | Q)</code>.</li>
 * </ul>
 * The rewriting rules executed herein do not make use of proxies, as fundamental types are not altered (i.e. only the
 * substance of the ConnectedSentenceNode operands are altered).
 *
 * @warning Although multiple passes are not required for this CNF-normalising visitor, it does recurse on any produced
 * terms to ensure a full reduction. On extremely deeply nested sentences, this could cause a machine stack overflow.
 */
class DisjunctionDistributionVisitor :
        public MutatingSentenceVisitorBase
{
public:
    /**
     * @brief Recursively applies disjunction-distribution to the connected sentence, in-place.
     * @param node The root of the connected sentence on which disjunction-distribution should be applied.
     */
    void visit(ConnectedSentenceNode &node) override;

    void reset() override;

private:
    /**
     * @enum TrackingState
     * @brief Indicate the current state of 'tracking', as required by a calling visitor.
     * @details When tracking is enabled (left- or right-major), operands/children of binary-connected nodes should
     *  be tracked by the DisjunctionDistributionVisitor instance.
     */
    enum class TrackingState
    {
        NotTracking, /**< Not tracking; nested children shouldn't record their operands. */
        LeftMajor, /**< Tracking to the left: nested children should record their left operands in the major slot */
        RightMajor /**< Tracking to the right: nested children should record their right operands in the major slot */
    };

    /**
     * @brief The current operand-tracking state
     */
    TrackingState tracking_state = TrackingState::NotTracking;

    /**
     * @brief The tracked operand stack stores, depth-wise, the major and minor child operands of connected nodes,
     *  respectively.
     */
    std::stack<std::pair<std::shared_ptr<ISentenceNode>, std::shared_ptr<ISentenceNode>>> tracked_operands;

    /**
     * @brief Apply any applicable reductions to the given node, given the collected tracked operands from eligible
     *  children.
     * @pre The given node must be of a disjunctive nature. The top of the tracked operands stack must be fully
     *  pending and not contain any sentinel containers.
     * @post The tracked operand stack is sentinel.
     * @param node The root node on which reduction should be applied
     * @return Was at least one reduction performed?
     */
    bool attempt_reduction(ConnectedSentenceNode &node);
};

}

#endif
