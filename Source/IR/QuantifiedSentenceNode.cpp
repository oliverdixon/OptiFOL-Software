//
// Created by owd on 24/11/24.
//

#include "QuantifiedSentenceNode.hpp"
#include "../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../Visitors/Sentences/IObservingSentenceVisitor.hpp"

namespace optifol
{

QuantifiedSentenceNode::QuantifiedSentenceNode(QuantifierTypes quantifier_type,
                                               std::shared_ptr<VariableNode> bound_variable,
                                               std::shared_ptr<ISentenceNode> sentence) :
        quantifier_type(quantifier_type),
        bound_variable(std::move(bound_variable)),
        sentence(std::move(sentence))
{}

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

void QuantifiedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

std::shared_ptr<ISentenceNode> QuantifiedSentenceNode::move_sentence()
{
    auto borrowed = std::move(sentence);
    sentence = nullptr;
    return borrowed;
}

void QuantifiedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

void QuantifiedSentenceNode::replace_bound_variable(std::shared_ptr<VariableNode> new_bound_variable)
{
    bound_variable = std::move(new_bound_variable);
}

}
