/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Term-Resolution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef TERMRESOLUTIONVISITOR_HPP
#define TERMRESOLUTIONVISITOR_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../../../IR/MutableVariants/Terms/IMutableTerm.hpp"
#include "MutatingTermVisitorBase.hpp"

namespace optifol
{

class MutableVariable;

class TermResolutionVisitor :
        public MutatingTermVisitorBase
{
public:
    explicit TermResolutionVisitor(
            const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>> &rewriting_rules_hook
    );

    void visit(MutableFunction &node) override;

private:
    const std::unordered_map<std::string, std::unique_ptr<IMutableTerm>>& rewriting_rules_hook;
};

}

#endif
