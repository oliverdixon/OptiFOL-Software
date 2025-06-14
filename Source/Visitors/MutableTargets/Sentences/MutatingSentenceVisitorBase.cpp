/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Mutating Sentence Visitor base class.
 * @author Oliver Dixon
 * @date 2024-11-24
 * @version Development
 */

#include "MutatingSentenceVisitorBase.hpp"

#include "../../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"

namespace optifol
{

void MutatingSentenceVisitorBase::visit(MutableQuantified &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    node.put_sentence(std::move(borrowed_sentence));
}

void MutatingSentenceVisitorBase::visit(MutableBinaryConnected &node)
{
    auto borrowed_operand = node.take_lhs_operand();
    borrowed_operand->accept(*this);
    node.put_lhs_operand(std::move(borrowed_operand));

    borrowed_operand = node.take_rhs_operand();
    borrowed_operand->accept(*this);
    node.put_rhs_operand(std::move(borrowed_operand));
}

void MutatingSentenceVisitorBase::visit(MutableIdentity &node)
{
    (void) node;
}

void MutatingSentenceVisitorBase::visit(MutablePredicate &node)
{
    (void) node;
}

void MutatingSentenceVisitorBase::visit(MutableSentenceRoot &node)
{
    auto borrowed_sentence = node.take_sentence();
    borrowed_sentence->accept(*this);
    node.put_sentence(std::move(borrowed_sentence));
}

}
