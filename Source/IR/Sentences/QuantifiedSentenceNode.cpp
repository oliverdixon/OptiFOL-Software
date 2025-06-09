/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Quantified Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "QuantifiedSentenceNode.hpp"
#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../Visitors/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

QuantifiedSentenceNode::QuantifiedSentenceNode(const QuantifierTypes quantifier_type,
                                               std::unique_ptr<ITermNode> &&bound_term,
                                               std::unique_ptr<ISentenceNode> &&sentence, const bool is_positive) :
    quantifier_type(quantifier_type), bound_term(std::move(bound_term)), sentence(std::move(sentence)),
    is_positive(is_positive)
{
}

std::unique_ptr<ISentenceNode> QuantifiedSentenceNode::clone() const
{
    return std::make_unique<QuantifiedSentenceNode>(quantifier_type, bound_term->clone(), sentence->clone(),
                                                    is_positive);
}

void QuantifiedSentenceNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool QuantifiedSentenceNode::is_negative_polarity() const
{
    return !is_positive;
}

void QuantifiedSentenceNode::set_quantifier_type(const QuantifierTypes quantifier_type)
{
    this->quantifier_type = quantifier_type;
}

QuantifierTypes QuantifiedSentenceNode::get_quantifier_type() const
{
    return quantifier_type;
}

const ITermNode *QuantifiedSentenceNode::observe_bound_term() const
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

const ISentenceNode *QuantifiedSentenceNode::observe_sentence() const
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

std::size_t QuantifiedSentenceNode::hash() const noexcept
{
    auto hash = std::hash<std::underlying_type_t<QuantifierTypes>>{}(
        static_cast<std::underlying_type_t<QuantifierTypes>>(quantifier_type));

    hash = hash_combine(hash, bound_term->hash());
    hash = hash_combine(hash, sentence->hash());

    return hash_polarity(hash, is_negative_polarity());
}

std::ostream &QuantifiedSentenceNode::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(' << TextSerialiserVisitor::get_operator_symbol(quantifier_type);
    bound_term->serialise(ostream);
    ostream << '(';
    sentence->serialise(ostream);
    return ostream << '(' << '(';
}

void QuantifiedSentenceNode::put_bound_term(std::unique_ptr<ITermNode> &&new_bound_term)
{
    bound_term = std::move(new_bound_term);
}

} // namespace optifol
