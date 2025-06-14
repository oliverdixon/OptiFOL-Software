/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Implication-Elimination Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-24
 * @version Development
 */

#ifndef IMPLICATIONELIMINATIONVISITOR_HPP
#define IMPLICATIONELIMINATIONVISITOR_HPP

#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

/**
 * @class ImplicationEliminationVisitor
 * @brief The Implication Elimination Visitor applies the first stage of in-situ model-rewriting to Negated Normal Form.
 *
 * @details The Implication Elimination Visitor rewrites the model to remove implications and equivalences. Implications
 * are rewritten as disjunctions, and equivalences are rewritten as conjunctions of disjunctions. In particular,
 * <ul>
 *  <li><code>P(x) =&gt; Q(x)</code> becomes <code>~P(x) | Q(x)</code>; and likewise, </li>
 *  <li><code>P(x) &lt;=&gt; Q(x)</code> becomes <code>(P(x) | ~Q(x)) & (~P(x) | Q(x))</code>.</li>
 * </ul>
 * The rewriting rules executed herein do not make use of proxies, as fundamental types are not altered (i.e. only the
 * substance of the MutableBinaryConnected operands are altered).
 */
class ImplicationEliminationVisitor :
        public MutatingSentenceVisitorBase
{
public:
    [[nodiscard]] std::string_view get_visitor_name() const override;

    /**
     * @brief Recursively applies implication-elimination transformations to the given connected sentence root
     * @param node The connected sentence root node on which the implication-elimination should be applied
     */
    void visit(MutableBinaryConnected &node) override;

private:
    static const char * visitor_name;
};

}

#endif
