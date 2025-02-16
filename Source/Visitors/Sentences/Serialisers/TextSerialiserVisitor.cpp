/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the text-serialising observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#include "TextSerialiserVisitor.hpp"

#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"
#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/NodeProxy.hpp"
#include "../../../IR/Sentences/NegatedSentenceNode.hpp"
#include "../../../IR/Sentences/IdentitySentenceNode.hpp"
#include "../../../IR/Sentences/PredicationNode.hpp"

namespace optifol
{

void TextSerialiserVisitor::visit(const QuantifiedSentenceNode &node)
{
    output_stream << get_operator_symbol(node.get_quantifier_type())
                  << node.get_bound_variable()->get_disambiguated_name() << ' ';

    node.get_sentence()->accept(*this);
}

void TextSerialiserVisitor::visit(const ConnectedSentenceNode &node)
{
    output_stream << '(';

    node.get_lhs_operand()->accept(*this);
    output_stream << get_operator_symbol(node.get_operator_type());
    node.get_rhs_operand()->accept(*this);

    output_stream << ')';
}

void TextSerialiserVisitor::visit(const NegatedSentenceNode &node)
{
    output_stream << '~' << ' ';
    node.get_operand()->accept(*this);
}

void TextSerialiserVisitor::visit(const NodeProxy &node)
{
    node.sentence->accept(*this);
}

void TextSerialiserVisitor::visit(const IdentitySentenceNode &node)
{
    output_stream << '(' << node.get_lhs_operand()->get_disambiguated_name() << ' ' << '=' << ' '
                  << node.get_rhs_operand()->get_disambiguated_name() << ')';
}

void TextSerialiserVisitor::visit(const PredicationNode &node)
{
    output_stream << node.name << '(';

    const auto argument_count = node.arguments.size();
    for (std::remove_const_t<decltype(argument_count)> i = 1; i < argument_count; ++i)
        output_stream << node.arguments[i - 1]->get_disambiguated_name() << ',' << ' ';

    if (argument_count > 0)
        output_stream << node.arguments[argument_count - 1]->get_disambiguated_name();

    output_stream << ')';
}

std::string TextSerialiserVisitor::extract()
{
    auto str = output_stream.str();
    std::ostringstream().swap(output_stream);
    return str;
}

const char *TextSerialiserVisitor::get_operator_symbol(BinaryOperatorTypes type)
{
    switch (type) {
    case BinaryOperatorTypes::Conjunction:
        return " & ";
    case BinaryOperatorTypes::Disjunction:
        return " | ";
    case BinaryOperatorTypes::Implication:
        return " => ";
    case BinaryOperatorTypes::Biconditional:
        return " <=> ";
    case BinaryOperatorTypes::None:
        return " ? ";
    }
}

const char *TextSerialiserVisitor::get_operator_symbol(QuantifierTypes type)
{
    switch (type) {
    case QuantifierTypes::Universal:
        return "ForAll ";
    case QuantifierTypes::Existential:
        return "ThereExists ";
    }
}

}
