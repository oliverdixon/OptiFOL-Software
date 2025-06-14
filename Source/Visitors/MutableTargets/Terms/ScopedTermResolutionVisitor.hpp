/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Scoped Term-Resolution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2025-05-08
 * @version Development
 */

#ifndef SCOPEDTERMRESOLUTIONVISITOR_HPP
#define SCOPEDTERMRESOLUTIONVISITOR_HPP

#include "TermResolutionVisitor.hpp"

namespace optifol
{

class ScopedTermResolutionVisitor :
        public TermResolutionVisitor
{
public:
    ScopedTermResolutionVisitor(
        const std::unordered_set<std::string> &scope_hook,
        const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>> &rewriting_rules_hook
    );

    void visit(MutableFunction &node) override;

    void visit(MutableVariable &node) override;

private:
    const std::unordered_set<std::string> &scope_hook;
};

}

#endif
