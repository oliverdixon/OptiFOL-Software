//
// Created by owd on 30/11/24.
//

#ifndef OPTIFOL_TERMRESOLUTIONVISITOR_HPP
#define OPTIFOL_TERMRESOLUTIONVISITOR_HPP

#include <string>
#include <unordered_set>
#include <optional>

#include "ITermVisitor.hpp"
#include "../../IR/QuantifiedSentenceNode.hpp"

namespace optifol
{

class TermResolutionVisitor :
        public ITermVisitor
{
public:
    void visit(FunctionNode &node) override;

    void visit(ConstantNode &node) override;

    void visit(VariableNode &node) override;

    void reset() override;

    [[nodiscard ("Check if the name was modified during disambiguation and propagate the change.")]]
    bool open_scope(QuantifiedSentenceNode& node);

    void close_scope(const VariableNode& node);

private:
    std::unordered_set<std::string> scope;

    std::unordered_set<std::string> adjacent;

    std::string generate_name(const std::string &name);

    unsigned int counter = 0;
};

}

#endif //OPTIFOL_TERMRESOLUTIONVISITOR_HPP
