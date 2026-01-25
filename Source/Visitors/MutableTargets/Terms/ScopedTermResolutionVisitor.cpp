/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Scoped Term-Resolution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2025-05-08
 * @version Development
 */

#include "ScopedTermResolutionVisitor.hpp"

#include "../../../Exceptions/SemanticException.hpp"
#include "../../../IR/MutableVariants/Terms/MutableVariable.hpp"

namespace optifol
{

ScopedTermResolutionVisitor::ScopedTermResolutionVisitor(const std::unordered_set<std::string> &scope_hook,
        const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>, StringHash, std::equal_to<>>
            &rewriting_rules_hook) :
    TermResolutionVisitor(rewriting_rules_hook),
    scope_hook(scope_hook)
{
}

void ScopedTermResolutionVisitor::visit(MutableFunction &node)
{
    TermResolutionVisitor::visit(node);
}

void ScopedTermResolutionVisitor::visit(MutableVariable &node)
{
    const auto &name = node.to_string();

    if (!scope_hook.contains(name))
        throw SemanticException("Referenced variable \"" + name + "\" is not defined in the current scope.");
}

} // namespace optifol
