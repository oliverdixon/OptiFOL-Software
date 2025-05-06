/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Universal-Elimination Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-25
 * @version Development
 */

#include "UniversalEliminationVisitor.hpp"

#include <cassert>

#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"

namespace optifol
{

void UniversalEliminationVisitor::visit(QuantifiedSentenceNode &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    if (extracted_sentence == nullptr)
        node.put_sentence(std::move(borrowed_sentence));
    else
        node.put_sentence(std::move(extracted_sentence));

    if (node.get_quantifier_type() == QuantifierTypes::Universal)
        extracted_sentence = std::move(node.take_sentence());
}

void UniversalEliminationVisitor::visit(ConnectedSentenceNode &node)
{
    auto borrowed_operand = node.take_lhs_operand();
    borrowed_operand->accept(*this);
    if (extracted_sentence == nullptr)
        node.put_lhs_operand(std::move(borrowed_operand));
    else
        node.put_lhs_operand(std::move(extracted_sentence));

    borrowed_operand = node.take_rhs_operand();
    borrowed_operand->accept(*this);
    if (extracted_sentence == nullptr)
        node.put_rhs_operand(std::move(borrowed_operand));
    else
        node.put_rhs_operand(std::move(extracted_sentence));
}

std::unique_ptr<ISentenceNode> UniversalEliminationVisitor::get_extracted_root()
{
    return std::move(extracted_sentence);
}

void UniversalEliminationVisitor::reset()
{
    // Ensure there's no sentence that was taken from a node and not returned.
    assert(extracted_sentence == nullptr);
}

}
