/*
* Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the De Morgan's Laws Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-27
 * @version Development
 */

#ifndef DMLVISITOR_HPP
#define DMLVISITOR_HPP

#include <memory>
#include <vector>

#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class IMutableSentence;

/**
 * @class DMLVisitor
 * @brief The DML (De Morgan's Laws) Visitor applies De Morgan's Laws with negative reduction to eligible sentences.
 *
 * @details The DML visitor recursively rewrites the root model with the following rules:
 * <ul>
 *   <li><code>~(P | Q)</code> becomes <code>~P & ~Q</code>;</li>
 *   <li><code>~(P & Q)</code> becomes <code>~P | ~Q</code>;</li>
 *   <li><code>~~P</code> becomes <code>P</code>;</li>
 *   <li><code>~%Ux(P(x))</code> becomes <code>%Ex(~P(x))</code>; and</li>
 *   <li><code>~%Ex(P(x))</code> becomes <code>%Ux(~P(x))</code>.</li>
 * </ul>
 *
 * @warning Although multiple passes are not required for this DML-normalising visitor, it does recurse on any produced
 * terms to ensure a full reduction. On extremely deeply nested sentences, this could cause a machine stack overflow.
 */
class DMLVisitor :
        public MutatingSentenceVisitorBase
{
public:
    [[nodiscard]] std::string_view get_visitor_name() const override;

    void visit(MutableBinaryConnected &node) override;

    void visit(MutableQuantified &node) override;

private:
    static const char * visitor_name;
};

}

#endif
