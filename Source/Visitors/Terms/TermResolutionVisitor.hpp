/**
 * @file TermResolutionVisitor.hpp
 * @brief Class specification for the Term-Resolution Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef OPTIFOL_TERMRESOLUTIONVISITOR_HPP
#define OPTIFOL_TERMRESOLUTIONVISITOR_HPP

#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "MutatingTermVisitorBase.hpp"

namespace optifol
{

class VariableNode;

class TermResolutionVisitor :
        public MutatingTermVisitorBase
{
public:
    TermResolutionVisitor(
            const std::unordered_set<std::string> &scope_hook,
            const std::unordered_map<std::string, std::shared_ptr<VariableNode>> &rewriting_rules_hook
    );

    void visit(FunctionNode &node) override;

    void visit(VariableNode &node) override;

private:
    const std::unordered_set<std::string> &scope_hook;

    const std::unordered_map<std::string, std::shared_ptr<VariableNode>> &rewriting_rules_hook;
};

}

#endif //OPTIFOL_TERMRESOLUTIONVISITOR_HPP
