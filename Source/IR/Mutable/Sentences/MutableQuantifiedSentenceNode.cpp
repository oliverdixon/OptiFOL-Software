/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Quantified Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableQuantifiedSentenceNode.hpp"
#include "../../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../../Visitors/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

MutableQuantifiedSentenceNode::MutableQuantifiedSentenceNode(const QuantifierTypes quantifier_type,
                                               std::unique_ptr<IMutableTermNode> &&bound_term,
                                               std::unique_ptr<IMutableSentenceNode> &&sentence, const bool is_positive) :
    quantifier_type(quantifier_type), bound_term(std::move(bound_term)), sentence(std::move(sentence)),
    is_positive(is_positive)
{
}

std::unique_ptr<IMutableSentenceNode> MutableQuantifiedSentenceNode::clone() const
{
    return std::make_unique<MutableQuantifiedSentenceNode>(quantifier_type, bound_term->clone(), sentence->clone(),
                                                    is_positive);
}

void MutableQuantifiedSentenceNode::flip_polarity()
{
    is_positive = !is_positive;
}

bool MutableQuantifiedSentenceNode::is_negative_polarity() const
{
    return !is_positive;
}

void MutableQuantifiedSentenceNode::set_quantifier_type(const QuantifierTypes quantifier_type)
{
    this->quantifier_type = quantifier_type;
}

QuantifierTypes MutableQuantifiedSentenceNode::get_quantifier_type() const
{
    return quantifier_type;
}

const IMutableTermNode *MutableQuantifiedSentenceNode::observe_bound_term() const
{
    return bound_term.get();
}

std::unique_ptr<IMutableTermNode> MutableQuantifiedSentenceNode::take_bound_term()
{
    return std::move(bound_term);
}

std::unique_ptr<IMutableSentenceNode> MutableQuantifiedSentenceNode::take_sentence()
{
    return std::move(sentence);
}

const IMutableSentenceNode *MutableQuantifiedSentenceNode::observe_sentence() const
{
    return sentence.get();
}

void MutableQuantifiedSentenceNode::put_sentence(std::unique_ptr<IMutableSentenceNode> &&sentence)
{
    this->sentence = std::move(sentence);
}

void MutableQuantifiedSentenceNode::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutableQuantifiedSentenceNode::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutableQuantifiedSentenceNode::hash() const noexcept
{
    auto hash = std::hash<std::underlying_type_t<QuantifierTypes>>{}(
        static_cast<std::underlying_type_t<QuantifierTypes>>(quantifier_type));

    hash = hash_combine(hash, bound_term->hash());
    hash = hash_combine(hash, sentence->hash());

    return hash_polarity(hash, is_negative_polarity());
}

std::ostream &MutableQuantifiedSentenceNode::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(' << TextSerialiserVisitor::get_operator_symbol(quantifier_type);
    bound_term->serialise(ostream);
    ostream << '(';
    sentence->serialise(ostream);
    return ostream << ')';
}

void MutableQuantifiedSentenceNode::put_bound_term(std::unique_ptr<IMutableTermNode> &&new_bound_term)
{
    bound_term = std::move(new_bound_term);
}

} // namespace optifol
