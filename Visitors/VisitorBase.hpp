//
// Created by owd on 23/11/24.
//

#ifndef OPTIFOL_VISITORBASE_HPP
#define OPTIFOL_VISITORBASE_HPP

#include "../IR/ConnectedSentenceNodeProxy.hpp"
#include "../IR/QuantifiedSentenceNodeProxy.hpp"
#include "../IR/IdentitySentenceNode.hpp"
#include "../IR/PredicationNode.hpp"
#include "../IR/NegatedSentenceNode.hpp"

namespace optifol
{

class VisitorBase
{
public:
    virtual void visit(QuantifiedSentenceNode& node)
    {
        node.get_sentence()->accept(*this);
    }

    virtual void visit(ConnectedSentenceNode& node)
    {
        node.get_lhs_operand()->accept(*this);
        node.get_rhs_operand()->accept(*this);
    }

    virtual void visit(IdentitySentenceNode& node) {}
    virtual void visit(PredicationNode& node) {}

    virtual void visit(NegatedSentenceNode& node)
    {
        node.get_operand()->accept(*this);
    }

    virtual void visit(QuantifiedSentenceNodeProxy& node)
    {
        node.sentence->accept(*this);
    }

    virtual void visit(ConnectedSentenceNodeProxy& node)
    {
        node.sentence->accept(*this);
    }
};

}

#endif //OPTIFOL_VISITORBASE_HPP
