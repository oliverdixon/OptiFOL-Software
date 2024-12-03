/**
 * @file ImplicationEliminationVisitor.hpp
 * @brief Class specification for the Implication-Elimination Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-24
 * @version Development
 */

#ifndef OPTIFOL_IMPLICATIONELIMINATIONVISITOR_HPP
#define OPTIFOL_IMPLICATIONELIMINATIONVISITOR_HPP

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
 * substance of the ConnectedSentenceNode operands are altered).
 */
class ImplicationEliminationVisitor :
        public MutatingSentenceVisitorBase
{
public:
    /**
     * @brief Recursively applies implication-elimination transformations to the given connected sentence root
     * @param node The connected sentence root node on which the implication-elimination should be applied
     */
    void visit(ConnectedSentenceNode &node) override;

    void reset() override;
};

}

#endif //OPTIFOL_IMPLICATIONELIMINATIONVISITOR_HPP
