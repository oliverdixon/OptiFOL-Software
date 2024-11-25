//
// Created by owd on 25/11/24.
//

#ifndef OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP
#define OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP

#include "VisitorBase.hpp"

namespace optifol
{

class UniversalEliminationVisitor :
        public VisitorBase
{
public:
    void visit(QuantifiedSentenceNode& node) override
    {
        VisitorBase::visit(node);

        if (node.get_quantifier_type() == QuantifierTypes::Universal)
            extracted_sentence = node.get_sentence();
    }

    void visit(QuantifiedSentenceNodeProxy& proxy) override
    {
        VisitorBase::visit(proxy);

        if (extracted_sentence != nullptr) {
            proxy.sentence = extracted_sentence;
            extracted_sentence = nullptr;
        }
    }

private:
    std::shared_ptr<ISentenceNode> extracted_sentence;
};

}

#endif //OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP
