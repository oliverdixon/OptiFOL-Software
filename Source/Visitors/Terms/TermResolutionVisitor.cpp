/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Term-Resolution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#include "TermResolutionVisitor.hpp"
#include "../../IR/Terms/FunctionNode.hpp"
#include "../../IR/Terms/VariableNode.hpp"
#include "../../Exceptions/SemanticException.hpp"

namespace optifol
{

TermResolutionVisitor::TermResolutionVisitor(
        const std::unordered_set<std::string> &scope_hook,
        std::unordered_map<std::string, std::unique_ptr<VariableNode>>& rewriting_rules_hook) :

        scope_hook(scope_hook), rewriting_rules_hook(rewriting_rules_hook)
{}

void TermResolutionVisitor::visit(FunctionNode &node)
{
    auto &args = node.get_arguments();
    const auto argument_count = args.size();

    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto &rule = rewriting_rules_hook.find(args[i]->get_disambiguated_name());
        if (rule != rewriting_rules_hook.end())
            args[i] = std::move(rule->second);

        args[i]->accept(*this);
    }
}

void TermResolutionVisitor::visit(VariableNode &node)
{
    const auto &name = node.to_string();

    if (!scope_hook.contains(name))
        throw SemanticException("Referenced variable \"" + name + "\" is not defined in the current scope.");
}

}
