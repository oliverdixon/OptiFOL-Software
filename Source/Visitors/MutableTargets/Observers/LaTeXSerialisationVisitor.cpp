/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/6/25.
//

#include "LaTeXSerialisationVisitor.hpp"

#include "../../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../../IR/MutableVariants/Sentences/MutableIdentity.hpp"
#include "../../../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../../../IR/MutableVariants/Terms/MutableConstant.hpp"
#include "../../../IR/MutableVariants/Terms/MutableFunction.hpp"
#include "../../../IR/MutableVariants/Terms/MutableVariable.hpp"

namespace optifol
{

void LaTeXSerialisationVisitor::visit(const MutableSentenceRoot &node)
{
    latex << '$';
    node.observe_sentence()->accept(*this);
    latex << '$';
}

void LaTeXSerialisationVisitor::visit(const MutableIdentity &node)
{
    node.observe_lhs_operand()->accept(*this);
    latex << "=";
    node.observe_rhs_operand()->accept(*this);
}

void LaTeXSerialisationVisitor::visit(const MutableBinaryConnected &node)
{
    node.observe_lhs_operand()->accept(*this);
    latex << get_connected_symbol(node.get_operator_type()) << ' ';
    node.observe_rhs_operand()->accept(*this);
}

void LaTeXSerialisationVisitor::visit(const MutableQuantified &node)
{
    latex << get_quantifier_symbol(node.get_quantifier_type()) << ' ';

    node.observe_bound_term()->accept(*this);
    latex << "\\left(";
    node.observe_sentence()->accept(*this);
    latex << "\\right)";
}

void LaTeXSerialisationVisitor::visit(const MutablePredicate &node)
{
    latex << node.get_name() << "\\left(";
    const auto &arguments = node.observe_arguments();

    if (!arguments.empty()) {
        const auto argument_count = arguments.size() - 1;

        for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx) {
            arguments[argument_idx]->accept(*this);
            latex << ',';
        }

        arguments[argument_count]->accept(*this);
    }

    latex << "\\right)";
}

void LaTeXSerialisationVisitor::visit(const MutableConstant &node)
{
    latex << node.to_string();
}

void LaTeXSerialisationVisitor::visit(const MutableFunction &node)
{
    latex << node.get_disambiguated_name() << "\\left(";
    const auto &arguments = node.observe_arguments();

    if (!arguments.empty()) {
        const auto argument_count = arguments.size() - 1;

        for (std::size_t argument_idx = 0; argument_idx < argument_count; ++argument_idx) {
            arguments[argument_idx]->accept(*this);
            latex << ',';
        }

        arguments[argument_count]->accept(*this);
    }

    latex << "\\right)";
}

void LaTeXSerialisationVisitor::visit(const MutableVariable &node)
{
    latex << node.to_string();
}

std::string LaTeXSerialisationVisitor::extract() const
{
    return latex.str();
}

const char *LaTeXSerialisationVisitor::get_quantifier_symbol(const QuantifierTypes quantifier) noexcept
{
    switch (quantifier) {
    case QuantifierTypes::Universal:
        return "\\forall";
    case QuantifierTypes::Existential:
        return "\\exists";
    }

    return "??";
}

const char *LaTeXSerialisationVisitor::get_connected_symbol(const BinaryOperatorTypes op) noexcept
{
    switch (op) {
    case BinaryOperatorTypes::Implication:
        return "\\implies";
    case BinaryOperatorTypes::Conjunction:
        return "\\land";
    case BinaryOperatorTypes::Disjunction:
        return "\\lor";
    case BinaryOperatorTypes::Biconditional:
        return "\\iff";
    }

    return "??";
}

} // namespace optifol
