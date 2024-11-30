//
// Created by owd on 29/11/24.
//

#include "TextSerialiserVisitor.hpp"

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

    const auto arg_count = node.arguments.size();
    for (auto i = 1; i < arg_count; ++i)
        output_stream << node.arguments[i - 1]->get_disambiguated_name() << ',' << ' ';

    if (arg_count > 0)
        output_stream << node.arguments[arg_count - 1]->get_disambiguated_name();

    output_stream << ')';
}

std::any TextSerialiserVisitor::extract() const
{
    return output_stream.str();
}

void TextSerialiserVisitor::reset()
{
    std::ostringstream().swap(output_stream);
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
