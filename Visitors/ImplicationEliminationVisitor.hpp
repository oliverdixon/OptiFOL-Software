/**
 * @file ImplicationEliminationVisitor.hpp
 * @brief Class specification for the Implication Purging Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-24
 * @version Development
 */

#ifndef OPTIFOL_IMPLICATIONELIMINATIONVISITOR_HPP
#define OPTIFOL_IMPLICATIONELIMINATIONVISITOR_HPP

#include "VisitorBase.hpp"

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
        public VisitorBase
{
public:
    void visit(ConnectedSentenceNode &node) override
    {
        VisitorBase::visit(node);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
        switch (node.get_operator_type()) {
            case BinaryOperatorTypes::Implication:
                node.operator_type = BinaryOperatorTypes::Disjunction;
                node.lhs = std::make_shared<NegatedSentenceNode>(node.lhs);
                break;

            case BinaryOperatorTypes::Biconditional:
                node.operator_type = BinaryOperatorTypes::Conjunction;
                const auto save_lhs = node.lhs;

                node.lhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                                   node.lhs,
                                                                   std::make_shared<NegatedSentenceNode>(node.rhs));

                node.rhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                                   std::make_shared<NegatedSentenceNode>(save_lhs),
                                                                   node.rhs);

                break;

        }
#pragma clang diagnostic pop
    }
};

}

#endif //OPTIFOL_IMPLICATIONELIMINATIONVISITOR_HPP
