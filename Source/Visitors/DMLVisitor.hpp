/**
 * @file DMLVisitor.hpp
 * @brief Class specification for the De Morgan's Laws Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-27
 * @version Development
 */

#ifndef OPTIFOL_DMLVISITOR_HPP
#define OPTIFOL_DMLVISITOR_HPP

#include <stack>

#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

/**
 * @class DMLVisitor
 * @brief The DML (De Morgan's Laws) Visitor applies De Morgan's Laws with negative reduction to eligible sentences.
 *
 * @details The DML visitor recursively rewrites the root model with the following rules:
 * <ul>
 *   <li><code>~(P | Q)</code> becomes <code>~P & ~Q</code>;</li>
 *   <li><code>~(P & Q)</code> becomes <code>~P | ~Q</code>;</li>
 *   <li><code>~~P</code> becomes <code>P</code>;</li>
 *   <li><code>~(%Ux(P(x)))</code> becomes <code>%Ex(~P(x))</code>; and</li>
 *   <li><code>~(%Ex(P(x)))</code> becomes <code>%Ux(~P(x))</code>.</li>
 * </ul>
 *
 * @warning Although multiple passes are not required for this DML-normalising visitor, it does recurse on any produced
 * terms to ensure a full reduction. On extremely deeply nested sentences, this could cause a machine stack overflow.
 */
class DMLVisitor :
        public VisitorBase
{
public:
    /**
     * @brief Initialises the DML visitor internal state
     */
    DMLVisitor();

    /**
     * @copydoc VisitorBase::visit(ConnectedSentenceNode&)
     * @pre The negative context stack must not be empty.
     * @post The negative context stack must not be empty.
     */
    void visit(ConnectedSentenceNode &node) override;

    /**
     * @copydoc VisitorBase::visit(QuantifiedSentenceNode&)
     * @pre The negative context stack must not be empty.
     * @post The negative context stack must not be empty.
     */
    void visit(QuantifiedSentenceNode &node) override;

    /**
     * @copydoc VisitorBase::visit(NegatedSentenceNode&)
     * @pre The negative context stack must not be empty.
     * @post The negative context stack must not be empty.
     */
    void visit(NegatedSentenceNode &node) override;

    /**
     * @copydoc VisitorBase::visit(NodeProxy&)
     * @pre The negative context stack must not be empty. Further, if the topmost layer the negative context stack has
     *  a populated ~P-type node (second entry), it must also have a populated P-type node (first node).
     * @post The negative context stack must not be empty.
     */
    void visit(NodeProxy &node) override;

    void reset() override;

private:
    /**
     * @brief The pending DML-transformed sentence
     *
     * @details The DML normalisers generally use this reference variable to store their most recent reduction. It is
     *  also useful as a sentinel (when comparing against the null pointer) to determine whether a DML-normalisation was
     *  performed at all.
     *
     * @warning This is highly state-based, and depends on trusted member functions undertaking appropriate reference-
     *  counting and clean-up. See the symbols referenced herein for examples of responsible usage.
     *
     * @sa DMLVisitor::visit(ConnectedSentenceNode&)
     * @sa DMLVisitor::visit(QuantifiedSentenceNode&)
     */
    std::shared_ptr<ISentenceNode> pending_dml;

    /**
     * @brief The negative context stack, composed of layers
     *
     * @details A negative context layer, represented as an element on the stack, denotes a boundary across which
     *  negation-reduction cannot translate. The first boolean flags whether the corresponding context layer is
     *  transiently positive (or negative, in the false case). The pair of pointers store references to the two
     *  irreducible forms of a chained negation of a sentence P(x): the positive branch P(x), and the negative branch
     *  ~P(x), respectively.
     *
     * @warning For each DML-normalisation operation, a new 'layer' of context must be provided. See the symbols
     *  referenced herein for examples of correct usage.
     *
     * @sa DMLVisitor::visit(ConnectedSentenceNode&)
     * @sa DMLVisitor::visit(QuantifiedSentenceNode&)
     */
    std::stack<std::pair<bool,
            std::pair<std::shared_ptr<ISentenceNode>, std::shared_ptr<ISentenceNode>>>> negative_context;
};

}

#endif //OPTIFOL_DMLVISITOR_HPP
