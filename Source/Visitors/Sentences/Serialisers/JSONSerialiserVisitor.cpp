/**
 * @file JSONSerialiserVisitor.cpp
 * @brief Class implementation for the JSON-serialising observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#include "JSONSerialiserVisitor.hpp"

#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"
#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/NodeProxy.hpp"
#include "../../../IR/Sentences/NegatedSentenceNode.hpp"
#include "../../../IR/Sentences/IdentitySentenceNode.hpp"
#include "../../../IR/Sentences/PredicationNode.hpp"

namespace optifol
{

void JSONSerialiserVisitor::visit(const QuantifiedSentenceNode &node)
{
    output_stream << R"({"type": "quantified", "nature": ")" << get_operator_symbol(node.get_quantifier_type()) <<
                  R"(", "variable": ")" << node.get_bound_variable()->get_disambiguated_name() << R"(", "sentence": )";

    node.get_sentence()->accept(*this);
    output_stream << '}';
}

void JSONSerialiserVisitor::visit(const ConnectedSentenceNode &node)
{
    output_stream << R"({"type": "binary", "operator": ")" << get_operator_symbol(node.get_operator_type()) <<
                  R"(", "left": )";

    node.get_lhs_operand()->accept(*this);
    output_stream << R"(, "right": )";

    node.get_rhs_operand()->accept(*this);
    output_stream << '}';
}

void JSONSerialiserVisitor::visit(const NegatedSentenceNode &node)
{
    output_stream << R"({"type": "negated", "content": )";
    node.get_operand()->accept(*this);
    output_stream << '}';
}

void JSONSerialiserVisitor::visit(const NodeProxy &node)
{
    node.sentence->accept(*this);
}

void JSONSerialiserVisitor::visit(const IdentitySentenceNode &node)
{
    output_stream << R"({"type": "equality", "left": ")" << node.get_lhs_operand()->get_disambiguated_name()
                  << R"(", "right": ")" <<
                  node.get_rhs_operand()->get_disambiguated_name() << '"' << '}';
}

void JSONSerialiserVisitor::visit(const PredicationNode &node)
{
    output_stream << R"({"type": "predicate", "name": ")" << node.name << R"(", "arguments": [)";

    const auto argument_count = node.arguments.size();
    for (std::remove_const_t<decltype(argument_count)> i = 1; i < argument_count; ++i)
        output_stream << '"' << node.arguments[i - 1]->get_disambiguated_name() << '"' << ',' << ' ';

    if (argument_count > 0)
        output_stream << '"' << node.arguments[argument_count - 1]->get_disambiguated_name() << '"';

    output_stream << ']' << '}';
}

std::any JSONSerialiserVisitor::extract() const
{
    return output_stream.str();
}

void JSONSerialiserVisitor::reset()
{
    std::ostringstream().swap(output_stream);
}

const char *JSONSerialiserVisitor::get_operator_symbol(BinaryOperatorTypes type)
{
    switch (type) {
        case BinaryOperatorTypes::Conjunction:
            return "conjunction";
        case BinaryOperatorTypes::Disjunction:
            return "disjunction";
        case BinaryOperatorTypes::Implication:
            return "implication";
        case BinaryOperatorTypes::Biconditional:
            return "biconditional";
        case BinaryOperatorTypes::None:
            return "none";
    }
}

const char *JSONSerialiserVisitor::get_operator_symbol(QuantifierTypes type)
{
    switch (type) {
        case QuantifierTypes::Universal:
            return "universal";
        case QuantifierTypes::Existential:
            return "existential";
    }
}

}
