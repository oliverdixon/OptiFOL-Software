//
// Created by owd on 30/11/24.
//

#ifndef OPTIFOL_SYMBOLVISITOR_HPP
#define OPTIFOL_SYMBOLVISITOR_HPP

#include <unordered_map>

#include "../MutatingVisitorBase.hpp"
#include "../../Terms/TermResolutionVisitor.hpp"

namespace optifol
{

class SymbolVisitor :
        public MutatingVisitorBase
{
public:
    void visit(QuantifiedSentenceNode &node) override;

    void visit(PredicationNode &node) override;

    void visit(IdentitySentenceNode &node) override;

    void reset() override;

private:
    TermResolutionVisitor term_visitor;

    // TODO: rewrite variables in functions (more complex, since functions are terms - so we can't use this visitor
    //  directly).
    std::unordered_map<std::string, std::shared_ptr<VariableNode>> rewriting_rules;
};

}

#endif //OPTIFOL_SYMBOLVISITOR_HPP
