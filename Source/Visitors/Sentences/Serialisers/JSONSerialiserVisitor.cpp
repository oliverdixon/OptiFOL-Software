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

#include "../../../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../../../IR/Sentences/IdentitySentenceNode.hpp"
#include "../../../IR/Sentences/PredicationNode.hpp"
#include "../../../IR/Sentences/QuantifiedSentenceNode.hpp"

namespace optifol
{

void JSONSerialiserVisitor::visit(const QuantifiedSentenceNode &node)
{
    // Get the sentence to build its JSON structure
    node.observe_sentence()->accept(*this);

    output = {
        { "type", "quantified" },
        { "nature", get_operator_symbol(node.get_quantifier_type()) },
        { "variable", node.observe_bound_term()->get_disambiguated_name() },
        { "sentence", std::move(output) }
    };
}

void JSONSerialiserVisitor::visit(const ConnectedSentenceNode &node)
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
}

void JSONSerialiserVisitor::visit(const IdentitySentenceNode &node)
{
    output = {
        { "type", "equality" },
        { "lhs", node.observe_lhs_operand()->get_disambiguated_name() },
        { "rhs", node.observe_rhs_operand()->get_disambiguated_name() }
    };
}

void JSONSerialiserVisitor::visit(const PredicationNode &node)
{
    nlohmann::json arguments = nlohmann::json::array();

    for (const auto& argument : node.arguments)
        arguments.push_back(argument->get_disambiguated_name());

    output = {
        { "type", "predicate" },
        { "name", node.name },
        { "arguments", std::move(arguments) }
    };
}

nlohmann::json JSONSerialiserVisitor::extract()
{
    auto output = std::move(this->output);
    this->output = nlohmann::json::object();
    return output; // TODO: check move NRVO is working
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

void JSONSerialiserVisitor::print_polarity(const ISentenceNode *node)
{
    if (node->is_negative_polarity())
        output.push_back({ "negative", true });
}

}
