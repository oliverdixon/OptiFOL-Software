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

#include "../MutatingVisitorBase.hpp"

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
 *   <li><code>~%Ux(P(x))</code> becomes <code>%Ex(~P(x))</code>; and</li>
 *   <li><code>~%Ex(P(x))</code> becomes <code>%Ux(~P(x))</code>.</li>
 * </ul>
 *
 * @warning Although multiple passes are not required for this DML-normalising visitor, it does recurse on any produced
 * terms to ensure a full reduction. On extremely deeply nested sentences, this could cause a machine stack overflow.
 */
class DMLVisitor :
        public MutatingVisitorBase
{
public:
    /**
     * @brief Initialises the DML visitor internal state
     */
    DMLVisitor();

    /**
     * @copydoc MutatingVisitorBase::visit(ConnectedSentenceNode&)
     */
    void visit(ConnectedSentenceNode &node) override;

    /**
     * @copydoc MutatingVisitorBase::visit(QuantifiedSentenceNode&)
     */
    void visit(QuantifiedSentenceNode &node) override;

    /**
     * @copydoc MutatingVisitorBase::visit(NegatedSentenceNode&)
     */
    void visit(NegatedSentenceNode &node) override;

    /**
     * @copydoc MutatingVisitorBase::visit(NodeProxy&)
     */
    void visit(NodeProxy &node) override;

    void reset() override;

private:
    struct ContextLayer
    {
        bool is_positive = true;
        std::shared_ptr<ISentenceNode> positive_branch;
        std::shared_ptr<ISentenceNode> negative_branch;
    };

    /**
     * @struct PendingTransformation
     * @brief A transparent container type for a pending DML transformation
     * @details The DML normalisers generally use a visitor-held instance of this structure to cache the most recent
     *  reduction. It is also useful as a sentinel (when comparing against the null pointer) to determine whether a
     *  DML-normalisation was performed at all.
     *
     * @warning This is highly state-based, and depends on trusted member functions undertaking appropriate reference-
     *  counting and clean-up. See the symbols referenced herein for examples of responsible usage.
     *
     * @sa DMLVisitor::visit(ConnectedSentenceNode&)
     * @sa DMLVisitor::visit(QuantifiedSentenceNode&)
     */
    struct PendingTransformation
    {
        /**
         * @brief The pending DML-transformed sentence
         */
        std::shared_ptr<ISentenceNode> pending_dml;

        /**
         * @brief The number of remaining 'skips' the pending DML-transformed sentence should undertake before
         * participating in a replacement.
         */
        unsigned int skip_node_count = 0;

        /**
         * @brief Is there a pending transformation suitable for despatch?
         */
        [[nodiscard]] bool pending() const;

        [[nodiscard("The stealer invalidates the internal state.")]] std::shared_ptr<ISentenceNode> steal();
    };

    PendingTransformation pending_transformation;

    /**
     * @brief The negative context stack, composed of layers
     *
     * @details A negative context layer, represented as an element on the stack, denotes a boundary across which
     *  negation-reduction cannot translate.

     * @warning For each DML-normalisation operation, a new 'layer' of context must be provided. See the symbols
     *  referenced herein for examples of correct usage.
     *
     * @sa DMLVisitor::visit(ConnectedSentenceNode&)
     * @sa DMLVisitor::visit(QuantifiedSentenceNode&)
     */
    std::stack<ContextLayer> negative_context;
};

}

#endif //OPTIFOL_DMLVISITOR_HPP
