/**
 * @file DMLVisitor.cpp
 * @brief Class implementation for the De Morgan's Laws Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-27
 * @version Development
 */

#include <cassert>

#include "DMLVisitor.hpp"

namespace optifol
{

DMLVisitor::DMLVisitor()
{
    negative_context.push({true, {nullptr, nullptr}});
}

void DMLVisitor::visit(ConnectedSentenceNode &node)
{
    assert(!negative_context.empty());
    const bool just_seen_negative = !negative_context.top().first;

    if (just_seen_negative) {
        const auto type = node.get_operator_type();

        if (type == BinaryOperatorTypes::Conjunction || type == BinaryOperatorTypes::Disjunction) {
            // TODO: can we std::move these? Need a non-const stealer in ConnectedSentenceNode.
            std::shared_ptr<ISentenceNode> lhs_neg = std::make_shared<NodeProxy>(
                    std::make_shared<NegatedSentenceNode>(node.get_lhs_operand()));
            std::shared_ptr<ISentenceNode> rhs_neg = std::make_shared<NodeProxy>(
                    std::make_shared<NegatedSentenceNode>(node.get_rhs_operand()));

            /* Negating the operands may introduce opportunities for further reduction; in particular, if the
             * operand was a conjunctive or disjunction binary-connected sentence. Recurse down on both sides, using
             * the 'pending DML' sentinel sentence to determine if further reduction took place. For second-order
             * RHS reductions, we can take the sentinel as an r-value reference to elide a shared_ptr copy. This is a
             * pain, and mandates nesting smart pointers inside proxies (since a ~~P-type node would be converted to
             * P-type node), but it allows us to avoid multiple full passes of the entire model.
             *
             * We also need new negative contexts for the L- and RHS, but that's nothing new from the non-DML case. */

            negative_context.push({true, {nullptr, nullptr}});
            lhs_neg->accept(*this);
            if (pending_dml != nullptr) {
                lhs_neg = std::move(pending_dml);
                pending_dml = nullptr;
            }

            negative_context.top() = {true, {nullptr, nullptr}};
            rhs_neg->accept(*this);
            negative_context.pop();

            pending_dml = std::make_shared<ConnectedSentenceNode>(
                    (type == BinaryOperatorTypes::Conjunction) ?
                    BinaryOperatorTypes::Disjunction :
                    BinaryOperatorTypes::Conjunction,
                    std::move(lhs_neg),
                    (pending_dml == nullptr) ? std::move(rhs_neg) : std::move(pending_dml));
        }
    } else {
        /* In the above branch, expressions produced are of the form (~P) | (~Q), or similar. L- and RHS are DML-
         * normalised before the outer connected sentence is constructed. Hence, there is no opportunity for further
         * DML normalisation. This branch emulates VisitorBase::visit(ConnectedSentenceNode&), taking care to provide
         * suitable negative context layers. */

        negative_context.push({true, {nullptr, nullptr}});
        node.get_lhs_operand()->accept(*this);
        negative_context.top() = {true, {nullptr, nullptr}};
        node.get_rhs_operand()->accept(*this);
        negative_context.pop();
    }

    assert(!negative_context.empty());
}

void DMLVisitor::visit(QuantifiedSentenceNode &node)
{
#if 0
    assert(!negative_context.empty());
    const bool just_seen_negative = !negative_context.top().first;

    if (just_seen_negative) {
        assert(pending_dml == nullptr);
        const auto type = node.get_quantifier_type();

        /* Negate the detained sentence within a proxy (due to a potential ~~P-type to P-type conversion). The detained
         * sentence is DML-normalised, so a layer of negation context is required. */
        // TODO: can we std::move this? Need a non-const stealer in QuantifiedSentenceNode.
        auto neg_operand = std::make_shared<NodeProxy>(
                std::make_shared<NegatedSentenceNode>(node.get_sentence()));

        negative_context.push({true, {nullptr, nullptr}});
        neg_operand->accept(*this);

        if (pending_dml != nullptr) {
            neg_operand->sentence = std::move(pending_dml);
            pending_dml = nullptr;
        }

        negative_context.pop();

        pending_dml = std::make_shared<QuantifiedSentenceNode>(
                (type == QuantifierTypes::Universal) ?
                QuantifierTypes::Existential :
                QuantifierTypes::Universal,
                node.get_bound_variable(), // TODO: we can definitely steal this
                std::move(neg_operand));
    } else {
        negative_context.push({true, {nullptr, nullptr}});
        VisitorBase::visit(node);
        negative_context.pop();
    }

    assert(!negative_context.empty());
#endif
}

void DMLVisitor::visit(NegatedSentenceNode &node)
{
    assert(!negative_context.empty());

    /* Grab the negative operands for our current level of context. Store the relevant operand (described below), and
     * flip the negation sentinel flag. The latter needs to happen before any recursive visitation, so our children
     * know that we're negative. */
    auto &context_layer = negative_context.top();
    context_layer.first = !context_layer.first;

    VisitorBase::visit(node);

    auto &negative_operands = context_layer.second;

    if (negative_operands.first == nullptr)
        /* If this is the first negated node in a consecutive chain ~...~P, we must be visiting precisely ~P. Therefore,
         * we save P in the first slot of the negated operand cache. */
        negative_operands.first = node.get_operand();

    else if (negative_operands.second == nullptr)
        /* If this is the second negated node in a consecutive chain ~...~P, we must be visiting precisely ~~P.
         * Therefore, we save ~P in the second slot of the negated operand cache. */
        negative_operands.second = node.get_operand();

    assert(!negative_context.empty());
}

void DMLVisitor::visit(NodeProxy &node)
{
    assert(!negative_context.empty());
    VisitorBase::visit(node);

    if (pending_dml != nullptr) {
        node.sentence = std::move(pending_dml);
        pending_dml = nullptr;
    }

    auto &negative_layer = negative_context.top().second;

    if (negative_layer.second != nullptr) {
        assert(negative_layer.first != nullptr);
        node.sentence = (negative_context.top().first) ? negative_layer.first : negative_layer.second;
        pending_dml = node.sentence;
    }

    assert(!negative_context.empty());
}

void DMLVisitor::reset()
{
    while (!negative_context.empty())
        negative_context.pop();

    negative_context.push({true, {nullptr, nullptr}});
}

}
