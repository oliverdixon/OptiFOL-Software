/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#include "QuantifiedSentenceNode.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

QuantifiedSentenceNode::QuantifiedSentenceNode(const QuantifierTypes quantifier_type,
                                               std::unique_ptr<VariableNode>&& bound_variable,
                                               std::unique_ptr<ISentenceNode>&& sentence) :
        quantifier_type(quantifier_type),
        bound_variable(std::move(bound_variable)),
        sentence(std::move(sentence))
{}

std::unique_ptr<ISentenceNode> QuantifiedSentenceNode::clone() const
{
    return std::make_unique<QuantifiedSentenceNode>(quantifier_type, bound_variable->clone(), sentence->clone());
}

QuantifierTypes QuantifiedSentenceNode::get_quantifier_type() const
{
    return quantifier_type;
}

std::unique_ptr<VariableNode> QuantifiedSentenceNode::get_bound_variable()
{
    return std::move(bound_variable);
}

std::unique_ptr<ISentenceNode> QuantifiedSentenceNode::get_sentence()
{
    return std::move(sentence);
}

void QuantifiedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

std::unique_ptr<ISentenceNode> QuantifiedSentenceNode::move_sentence()
{
    auto borrowed = std::move(sentence);
    sentence = nullptr;
    return borrowed;
}

void QuantifiedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

void QuantifiedSentenceNode::swap_bound_variable(std::unique_ptr<VariableNode>&& new_bound_variable)
{
    bound_variable.swap(new_bound_variable);
}

}
