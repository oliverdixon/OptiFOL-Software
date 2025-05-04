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
                                               std::unique_ptr<ITermNode>&& bound_term,
                                               std::unique_ptr<ISentenceNode>&& sentence) :
        quantifier_type(quantifier_type),
        bound_term(std::move(bound_term)),
        sentence(std::move(sentence))
{}

std::unique_ptr<ISentenceNode> QuantifiedSentenceNode::clone() const
{
    return std::make_unique<QuantifiedSentenceNode>(quantifier_type, bound_term->clone(), sentence->clone());
}

QuantifierTypes QuantifiedSentenceNode::get_quantifier_type() const
{
    return quantifier_type;
}

const ITermNode * QuantifiedSentenceNode::observe_bound_term() const
{
    return bound_term.get();
}

std::unique_ptr<ITermNode> QuantifiedSentenceNode::take_bound_term()
{
    return std::move(bound_term);
}

std::unique_ptr<ISentenceNode> QuantifiedSentenceNode::take_sentence()
{
    return std::move(sentence);
}

const ISentenceNode * QuantifiedSentenceNode::observe_sentence() const
{
    return sentence.get();
}

void QuantifiedSentenceNode::put_sentence(std::unique_ptr<ISentenceNode> &&sentence)
{
    this->sentence = std::move(sentence);
}

void QuantifiedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void QuantifiedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

void QuantifiedSentenceNode::swap_bound_term(std::unique_ptr<ITermNode>&& new_bound_term)
{
    bound_term.swap(new_bound_term);
}

}
