//
// Created by owd on 24/11/24.
//

#include "QuantifiedSentenceNode.hpp"
#include "../Visitors/VisitorBase.hpp"

namespace optifol
{

QuantifiedSentenceNode::QuantifiedSentenceNode(QuantifierTypes quantifier_type,
                                               std::shared_ptr<VariableNode> bound_variable,
                                               std::shared_ptr<ISentenceNode> sentence) :
        quantifier_type(quantifier_type),
        bound_variable(std::move(bound_variable)),
        sentence(std::move(sentence))
{}

std::string QuantifiedSentenceNode::to_string() const
{
    return ((quantifier_type == QuantifierTypes::Universal) ? "ForAll " : "ThereExist ") +
           bound_variable->to_string() + " (" + sentence->to_string() + ')';
}

QuantifierTypes QuantifiedSentenceNode::get_quantifier_type() const
{
    return quantifier_type;
}

std::shared_ptr<VariableNode> QuantifiedSentenceNode::get_bound_variable() const
{
    return bound_variable;
}

std::shared_ptr<ISentenceNode> QuantifiedSentenceNode::get_sentence() const
{
    return sentence;
}

void QuantifiedSentenceNode::accept(VisitorBase &visitor)
{
    visitor.visit(*this);
}

std::shared_ptr<ISentenceNode> QuantifiedSentenceNode::move_sentence()
{
    auto borrowed = std::move(sentence);
    sentence = nullptr;
    return borrowed;
}

}
