/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Disjunction-Distribution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-25
 * @version Development
 */

#include <cassert>

#include "../../../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "DisjunctionDistributionVisitor.hpp"

namespace optifol
{

const char *DisjunctionDistributionVisitor::visitor_name = "DisjunctionDistribution";

std::string_view DisjunctionDistributionVisitor::get_visitor_name() const
{
    return visitor_name;
}

void DisjunctionDistributionVisitor::visit(MutableBinaryConnected &node)
{
    tracking_mode = TrackingMode::NotTracking;

    while (!tracked_operands.empty())
        tracked_operands.pop();

    MutatingSentenceVisitorBase::visit(node);

    if (node.get_operator_type() == BinaryOperatorTypes::Disjunction)
        attempt_reduction(node);
}

bool DisjunctionDistributionVisitor::attempt_reduction(MutableBinaryConnected &node)
{
    assert(node.get_operator_type() == BinaryOperatorTypes::Disjunction);

    tracking_mode = TrackingMode::NotTracking;

    while (!tracked_operands.empty())
        tracked_operands.pop();

    auto lhs = node.take_lhs_operand();
    auto rhs = node.take_rhs_operand();

    assert(lhs);
    assert(rhs);

    /*
     * Case 1:
     * A | (B & C) becomes (A | C) & (A | B)
     */
    if (auto *rhs_binary = dynamic_cast<MutableBinaryConnected *>(rhs.get());
        rhs_binary && rhs_binary->get_operator_type() == BinaryOperatorTypes::Conjunction) {

        auto rhs_lhs = rhs_binary->take_lhs_operand();
        auto rhs_rhs = rhs_binary->take_rhs_operand();

        assert(rhs_lhs);
        assert(rhs_rhs);

        auto lhs_clone = lhs->clone();

        node.set_operator_type(BinaryOperatorTypes::Conjunction);

        node.put_lhs_operand(std::make_unique<MutableBinaryConnected>(
                BinaryOperatorTypes::Disjunction,
                std::move(lhs),
                std::move(rhs_rhs)));

        node.put_rhs_operand(std::make_unique<MutableBinaryConnected>(
                BinaryOperatorTypes::Disjunction,
                std::move(lhs_clone),
                std::move(rhs_lhs)));

        MutatingSentenceVisitorBase::visit(node);
        return true;
    }

    /*
     * Case 2:
     * (A & B) | C becomes (C | A) & (C | B)
     */
    if (auto *lhs_binary = dynamic_cast<MutableBinaryConnected *>(lhs.get());
        lhs_binary && lhs_binary->get_operator_type() == BinaryOperatorTypes::Conjunction) {

        auto lhs_lhs = lhs_binary->take_lhs_operand();
        auto lhs_rhs = lhs_binary->take_rhs_operand();

        assert(lhs_lhs);
        assert(lhs_rhs);

        auto rhs_clone = rhs->clone();

        node.set_operator_type(BinaryOperatorTypes::Conjunction);

        node.put_lhs_operand(std::make_unique<MutableBinaryConnected>(
                BinaryOperatorTypes::Disjunction,
                std::move(rhs),
                std::move(lhs_lhs)));

        node.put_rhs_operand(std::make_unique<MutableBinaryConnected>(
                BinaryOperatorTypes::Disjunction,
                std::move(rhs_clone),
                std::move(lhs_rhs)));

        MutatingSentenceVisitorBase::visit(node);
        return true;
    }

    node.put_lhs_operand(std::move(lhs));
    node.put_rhs_operand(std::move(rhs));

    return false;
}

} // namespace optifol
