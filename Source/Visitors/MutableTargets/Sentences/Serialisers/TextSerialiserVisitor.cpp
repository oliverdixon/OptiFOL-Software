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

#include "../../../../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableIdentity.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../../../../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"

namespace optifol
{

void TextSerialiserVisitor::visit(const MutableQuantified &node)
{
    print_polarity(&node);
    output_stream << Quantified::get_operator_symbol(node.get_quantifier_type())
                  << node.observe_bound_term()->to_string() << ' ';

    node.observe_sentence()->accept(*this);
}

void TextSerialiserVisitor::visit(const MutableBinaryConnected &node)
{
    print_polarity(&node);
    output_stream << '(';

    node.observe_lhs_operand()->accept(*this);
    output_stream << BinaryConnected::get_operator_symbol(node.get_operator_type());
    node.observe_rhs_operand()->accept(*this);

    output_stream << ')';
}

void TextSerialiserVisitor::visit(const MutableIdentity &node)
{
    print_polarity(&node);
    output_stream << '(' << node.observe_lhs_operand()->to_string() << ' ' << '=' << ' '
                  << node.observe_rhs_operand()->to_string() << ')';
}

void TextSerialiserVisitor::visit(const MutablePredicate &node)
{
    const auto& arguments = node.observe_arguments();
    print_polarity(&node);
    output_stream << node.get_name() << '(';

    const auto argument_count = arguments.size();
    for (std::remove_const_t<decltype(argument_count)> i = 1; i < argument_count; ++i)
        output_stream << arguments[i - 1]->to_string() << ',' << ' ';

    if (argument_count > 0)
        output_stream << arguments[argument_count - 1]->to_string();

    output_stream << ')';
}

void TextSerialiserVisitor::visit(const MutableSentenceRoot &node)
{
    node.observe_sentence()->accept(*this);
}

std::string TextSerialiserVisitor::extract()
{
    auto str = output_stream.str();
    std::ostringstream().swap(output_stream);
    return str;
}

void TextSerialiserVisitor::print_polarity(const IMutableSentence *node)
{
    if (node->is_negative_polarity())
        output_stream << '~';
}

}
