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
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"
#include "../../../IR/Sentences/NodeProxy.hpp"

namespace optifol
{

void UniversalEliminationVisitor::visit(QuantifiedSentenceNode &node)
{
    MutatingSentenceVisitorBase::visit(node);

    if (node.get_quantifier_type() == QuantifierTypes::Universal)
        extracted_sentence = node.move_sentence();
}

void UniversalEliminationVisitor::visit(NodeProxy &proxy)
{
    MutatingSentenceVisitorBase::visit(proxy);

    if (extracted_sentence != nullptr) {
        proxy.sentence = std::move(extracted_sentence);
        extracted_sentence = nullptr;
    }
}

void UniversalEliminationVisitor::reset()
{
    extracted_sentence = nullptr;
}

}
