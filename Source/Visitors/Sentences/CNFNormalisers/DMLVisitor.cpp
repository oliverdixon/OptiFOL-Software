/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the De Morgan's Laws Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-27
 * @version Development
 */

#include <cassert>

#include "DMLVisitor.hpp"
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"
#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/NegatedSentenceNode.hpp"
#include "../../../IR/Sentences/NodeProxy.hpp"

namespace optifol
{

DMLVisitor::DMLVisitor()
{
    negative_context.emplace();
}

void DMLVisitor::visit(ConnectedSentenceNode &node)
{
    assert(!negative_context.empty());

    if (!negative_context.top().is_positive) {
        const auto type = node.get_operator_type();

        if (type == BinaryOperatorTypes::Conjunction || type == BinaryOperatorTypes::Disjunction) {
            // Negate both operands and put them in a proxy.
            std::unique_ptr<ISentenceNode> lhs_neg = std::make_unique<NodeProxy>(
                std::make_unique<NegatedSentenceNode>(node.take_lhs_operand()));
            std::unique_ptr<ISentenceNode> rhs_neg = std::make_unique<NodeProxy>(
                std::make_unique<NegatedSentenceNode>(node.take_rhs_operand()));

            /* Negating the operands may introduce opportunities for further reduction; in particular, if the
             * operand was a conjunctive or disjunction binary-connected sentence. Recurse down on both sides, using
             * the 'pending DML' sentinel sentence to determine if further reduction took place. For second-order
             * RHS reductions, we can take the sentinel as an r-value reference to elide a shared_ptr copy. This is a
             * pain, and mandates nesting smart pointers inside proxies (since a ~~P-type node would be converted to
             * P-type node), but it allows us to avoid multiple full passes of the entire model.
             *
             * We also need new negative contexts for the L- and RHS, but that's nothing new from the non-DML case. */

            negative_context.emplace();
            lhs_neg->accept(*this);
            if (pending_transformation.pending())
                lhs_neg = pending_transformation.steal();

            negative_context.pop();
            negative_context.emplace();

            rhs_neg->accept(*this);
            if (pending_transformation.pending())
                rhs_neg = pending_transformation.steal();

            negative_context.pop();

            pending_transformation.pending_dml = std::make_unique<ConnectedSentenceNode>(
                    (type == BinaryOperatorTypes::Conjunction) ?
                    BinaryOperatorTypes::Disjunction :
                    BinaryOperatorTypes::Conjunction,
                    std::move(lhs_neg),
                    std::move(rhs_neg));

            pending_transformation.skip_node_count = 0;
        }
    } else {
        /* In the above branch, expressions produced are of the form (~P) | (~Q), or similar. L- and RHS are DML-
         * normalised before the outer connected sentence is constructed. Hence, there is no opportunity for further
         * DML normalisation. This branch emulates MutatingSentenceVisitorBase::visit(ConnectedSentenceNode&), taking
         * care to provide suitable negative context layers. */

        negative_context.emplace();
        node.take_lhs_operand()->accept(*this);
        negative_context.pop();

        negative_context.emplace();
        node.take_rhs_operand()->accept(*this);
        negative_context.pop();
    }

    assert(!negative_context.empty());
}

void DMLVisitor::visit(QuantifiedSentenceNode &node)
{
    assert(!negative_context.empty());

    if (!negative_context.top().is_positive) {
        assert(!pending_transformation.pending());
        const auto type = node.get_quantifier_type();

        /* Negate the detained sentence within a proxy (due to a potential ~~P-type to P-type conversion). The detained
         * sentence is DML-normalised, so a layer of negation context is required. */
        auto neg_operand = std::make_unique<NodeProxy>(
                std::make_unique<NegatedSentenceNode>(node.take_sentence()));

        negative_context.emplace();
        neg_operand->accept(*this);

        if (pending_transformation.pending())
            neg_operand->sentence = pending_transformation.steal();

        negative_context.pop();

        pending_transformation.pending_dml = std::make_unique<QuantifiedSentenceNode>(
                type == QuantifierTypes::Universal ? QuantifierTypes::Existential : QuantifierTypes::Universal,
                node.take_bound_term(),
                std::move(neg_operand));

        pending_transformation.skip_node_count = 1;
    } else {
        negative_context.emplace();
        MutatingSentenceVisitorBase::visit(node);
        negative_context.pop();
    }

    assert(!negative_context.empty());
}

void DMLVisitor::visit(NegatedSentenceNode &node)
{
    assert(!negative_context.empty());

    /* Grab the negative operands for our current level of context. Store the relevant operand (described below), and
     * flip the negation sentinel flag. The latter needs to happen before any recursive visitation, so our children
     * know that we're negative. */
    auto &context_layer = negative_context.top();
    context_layer.is_positive = !context_layer.is_positive;

    MutatingSentenceVisitorBase::visit(node);

    if (context_layer.positive_branch == nullptr)
        /* If this is the first negated node in a consecutive chain ~...~P, we must be visiting precisely ~P. Therefore,
         * we save P in the first slot of the negated operand cache. */
        context_layer.positive_branch = node.take_operand();

    else if (context_layer.negative_branch == nullptr)
        /* If this is the second negated node in a consecutive chain ~...~P, we must be visiting precisely ~~P.
         * Therefore, we save ~P in the second slot of the negated operand cache. */
        context_layer.negative_branch = node.take_operand();

    assert(!negative_context.empty());
}

void DMLVisitor::visit(NodeProxy &node)
{
    assert(!negative_context.empty());
    MutatingSentenceVisitorBase::visit(node);

    if (pending_transformation.pending()) {
        if (pending_transformation.skip_node_count == 0)
            node.sentence = pending_transformation.steal();
        else
            --pending_transformation.skip_node_count;
    }

    auto &layer = negative_context.top();

    if (layer.negative_branch != nullptr) {
        assert(layer.positive_branch != nullptr);
        node.sentence = layer.is_positive ? std::move(layer.positive_branch) : std::move(layer.negative_branch);
        pending_transformation.pending_dml = std::move(node.sentence);
        pending_transformation.skip_node_count = 0;
    }

    assert(!negative_context.empty());
}

void DMLVisitor::reset()
{
    pending_transformation.pending_dml = nullptr;
    pending_transformation.skip_node_count = 0;

    while (!negative_context.empty())
        negative_context.pop();

    negative_context.emplace();
}

bool DMLVisitor::PendingTransformation::pending() const
{
    return pending_dml != nullptr;
}

std::unique_ptr<ISentenceNode> DMLVisitor::PendingTransformation::steal()
{
    auto ptr = std::move(pending_dml);
    pending_dml = nullptr;
    skip_node_count = 0;
    return ptr;
}

}
