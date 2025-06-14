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
#include "../../../IR/MutableVariants/Terms/MutableFunction.hpp"

namespace optifol
{

TermResolutionVisitor::TermResolutionVisitor(
        const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>> & rewriting_rules_hook) :
    rewriting_rules_hook(rewriting_rules_hook)
{}

void TermResolutionVisitor::visit(MutableFunction &node)
{
    auto &args = node.observe_arguments();
    const auto argument_count = args.size();

    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto &rule = rewriting_rules_hook.find(args[i]->
            get_disambiguated_name());
        if (rule != rewriting_rules_hook.end())
            args[i] = rule->second->clone();

        args[i]->accept(*this);
    }
}

}
