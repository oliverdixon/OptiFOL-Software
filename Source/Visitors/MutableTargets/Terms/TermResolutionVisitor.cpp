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
        const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>, StringHash, std::equal_to<>>
            &rewriting_rules_hook) :
    rewriting_rules_hook(rewriting_rules_hook)
{
}

void TermResolutionVisitor::visit(MutableFunction &node)
{
    auto &args = node.observe_arguments();
    const auto argument_count = args.size();

    for (std::remove_const_t<decltype(argument_count)> i = 0; i < argument_count; ++i) {
        const auto &rule = rewriting_rules_hook.find(args[i]->get_disambiguated_name());
        if (rule != rewriting_rules_hook.end() && node != *rule->second)
            /*
             * Rewrite the argument according to the rule if and only if:
             *
             *  - A suitable rule is available, such that the disambiguated name of the argument has been identified as
             *    rewritable; and
             *  - The rewriting would change the argument. If this check is not done, an infinite loop would be caused
             *    by the following accept call.
             */
            args[i] = rule->second->clone();

        args[i]->accept(*this);
    }
}

} // namespace optifol
