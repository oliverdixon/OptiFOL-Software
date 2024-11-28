//
// Created by owd on 25/11/24.
//

#include "UniversalEliminationVisitor.hpp"

namespace optifol
{

void UniversalEliminationVisitor::visit(QuantifiedSentenceNode &node)
{
    VisitorBase::visit(node);

    if (node.get_quantifier_type() == QuantifierTypes::Universal)
        extracted_sentence = node.move_sentence();
}

void UniversalEliminationVisitor::visit(NodeProxy &proxy)
{
    VisitorBase::visit(proxy);

    if (extracted_sentence != nullptr) {
        proxy.sentence = std::move(extracted_sentence);
        extracted_sentence = nullptr;
    }
}

void UniversalEliminationVisitor::reset()
{
    extracted_sentence = nullptr;
}

}
