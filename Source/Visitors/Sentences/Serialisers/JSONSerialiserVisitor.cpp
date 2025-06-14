/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the JSON-serialising observing visitor.
 * @author Oliver Dixon
 * @date 2024-11-29
 * @version Development
 */

#include "JSONSerialiserVisitor.hpp"

#include "../../../IR/Mutable/Sentences/MutableConnectedSentenceNode.hpp"
#include "../../../IR/Mutable/Sentences/MutableIdentitySentenceNode.hpp"
#include "../../../IR/Mutable/Sentences/MutablePredicationNode.hpp"
#include "../../../IR/Mutable/Sentences/MutableQuantifiedSentenceNode.hpp"
#include "../../../IR/Mutable/Sentences/MutableSentenceRoot.hpp"

namespace optifol
{

void JSONSerialiserVisitor::visit(const MutableQuantifiedSentenceNode &node)
{
    // Get the sentence to build its JSON structure
    node.observe_sentence()->accept(*this);

    output = {
        { "type", "quantified" },
        { "nature", get_operator_symbol(node.get_quantifier_type()) },
        { "variable", node.observe_bound_term()->to_string() },
        { "sentence", std::move(output) }
    };

    print_polarity(&node);
}

void JSONSerialiserVisitor::visit(const MutableConnectedSentenceNode &node)
{
    // Get the LHS to build its JSON structure, and move the entire structure into the local scope
    node.observe_lhs_operand()->accept(*this);
    nlohmann::json lhs = std::move(output);

    // Get the RHS to build its JSON structure, and move the entire structure into the local scope
    node.observe_rhs_operand()->accept(*this);
    nlohmann::json rhs = std::move(output);

    // Use the moved operand structures to rebuild the output
    output = {
        { "type", "binary" },
        { "operator", get_operator_symbol(node.get_operator_type()) },
        { "lhs", std::move(lhs) },
        { "rhs", std::move(rhs) }
    };

    print_polarity(&node);
}

void JSONSerialiserVisitor::visit(const MutableIdentitySentenceNode &node)
{
    output = {
        { "type", "equality" },
        { "lhs", node.observe_lhs_operand()->to_string() },
        { "rhs", node.observe_rhs_operand()->to_string() }
    };

    print_polarity(&node);
}

void JSONSerialiserVisitor::visit(const MutablePredicationNode &node)
{
    nlohmann::json arguments = nlohmann::json::array();

    for (const auto& argument : node.arguments)
        arguments.push_back(argument->to_string());

    output = {
        { "type", "predicate" },
        { "name", node.name },
        { "arguments", std::move(arguments) }
    };

    print_polarity(&node);
}

void JSONSerialiserVisitor::visit(const MutableSentenceRoot &node)
{
    node.observe_sentence()->accept(*this);
}

nlohmann::json JSONSerialiserVisitor::extract()
{
    auto output = std::move(this->output);
    this->output = nlohmann::json::object();
    return output; // TODO: check move NRVO is working
}

const char *JSONSerialiserVisitor::get_operator_symbol(const BinaryOperatorTypes type)
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

const char *JSONSerialiserVisitor::get_operator_symbol(const QuantifierTypes type)
{
    switch (type) {
    case QuantifierTypes::Universal:
        return "universal";
    case QuantifierTypes::Existential:
        return "existential";
    }
}

void JSONSerialiserVisitor::print_polarity(const IMutableSentenceNode *node)
{
    if (node->is_negative_polarity())
        output.push_back({ "negative", true });
}

}
