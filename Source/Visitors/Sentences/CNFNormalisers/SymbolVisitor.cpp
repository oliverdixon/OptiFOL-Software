//
// Created by owd on 30/11/24.
//

#include "SymbolVisitor.hpp"

namespace optifol
{

void SymbolVisitor::visit(QuantifiedSentenceNode &node)
{
    const auto &rule = rewriting_rules.find(node.get_bound_variable()->get_disambiguated_name());
    if (rule != rewriting_rules.cend())
        node.replace_bound_variable(rule->second);

    if (term_visitor.open_scope(node)) {
        auto variable = node.get_bound_variable();
        rewriting_rules.emplace(variable->to_string(), std::move(variable));
    }

    const auto &variable = *node.get_bound_variable();
    MutatingVisitorBase::visit(node);

    rewriting_rules.erase(variable.to_string());
    term_visitor.close_scope(variable);
}

void SymbolVisitor::visit(PredicationNode &node)
{
    auto &args = node.arguments;
    const auto argument_count = args.size();

    for (auto i = 0; i < argument_count; ++i) {
        const auto& rule = rewriting_rules.find(args[i]->get_disambiguated_name());
        if (rule != rewriting_rules.cend())
            args[i] = rule->second;

        args[i]->accept(term_visitor);
    }

    MutatingVisitorBase::visit(node);
}

void SymbolVisitor::visit(IdentitySentenceNode &node)
{
    const auto& lhs_operand = node.get_lhs_operand();
    const auto& rhs_operand = node.get_rhs_operand();

    const auto& lhs_rule = rewriting_rules.find(lhs_operand->get_disambiguated_name());
    if (lhs_rule != rewriting_rules.cend())
        node.swap_lhs_operand(lhs_rule->second);

    const auto& rhs_rule = rewriting_rules.find(rhs_operand->get_disambiguated_name());
    if (rhs_rule != rewriting_rules.cend())
        node.swap_rhs_operand(rhs_rule->second);

    node.get_lhs_operand()->accept(term_visitor);
    node.get_rhs_operand()->accept(term_visitor);

    MutatingVisitorBase::visit(node);
}

void SymbolVisitor::reset()
{
    term_visitor.reset();
}

}
