//
// Created by owd on 23/11/24.
//

#ifndef OPTIFOL_IMPLICATIONPURGINGVISITOR_HPP
#define OPTIFOL_IMPLICATIONPURGINGVISITOR_HPP

#include "VisitorBase.hpp"

#include "../IR/QuantifiedSentenceNodeProxy.hpp"
#include "../IR/ConnectedSentenceNodeProxy.hpp"

namespace optifol
{

class ImplicationPurgingVisitor :
        public VisitorBase
{
public:
    void visit(ConnectedSentenceNode &node) override
    {
        VisitorBase::visit(node);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
        switch (node.get_operator_type()) {
            case BinaryOperatorTypes::Implication:
                node.operator_type = BinaryOperatorTypes::Disjunction;
                node.lhs = std::make_shared<NegatedSentenceNode>(node.lhs);
                break;

            case BinaryOperatorTypes::Biconditional:
                node.operator_type = BinaryOperatorTypes::Conjunction;
                const auto save_lhs = node.lhs;

                node.lhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                                   node.lhs,
                                                                   std::make_shared<NegatedSentenceNode>(node.rhs));

                node.rhs = std::make_shared<ConnectedSentenceNode>(BinaryOperatorTypes::Disjunction,
                                                                   std::make_shared<NegatedSentenceNode>(save_lhs),
                                                                   node.rhs);

                break;

        }
#pragma clang diagnostic pop
    }
};

}

#endif //OPTIFOL_IMPLICATIONPURGINGVISITOR_HPP
