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

#include "MutableQuantified.hpp"
#include "../../../Visitors/MutableTargets/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../../Visitors/MutableTargets/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

MutableQuantified::MutableQuantified(const QuantifierTypes quantifier_type,
                                               std::unique_ptr<IMutableTerm> &&bound_term,
                                               std::unique_ptr<IMutableSentence> &&sentence, const bool is_positive) :
    quantifier_type(quantifier_type), bound_term(std::move(bound_term)), sentence(std::move(sentence)),
    is_positive(is_positive)
{
}

std::unique_ptr<IMutableSentence> MutableQuantified::clone() const
{
    return std::make_unique<MutableQuantified>(quantifier_type, bound_term->clone(), sentence->clone(),
                                                    is_positive);
}

void MutableQuantified::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool MutableQuantified::is_negative_polarity() const noexcept
{
    return !is_positive;
}

void MutableQuantified::set_quantifier_type(const QuantifierTypes quantifier_type)
{
    this->quantifier_type = quantifier_type;
}

QuantifierTypes MutableQuantified::get_quantifier_type() const
{
    return quantifier_type;
}

const IMutableTerm *MutableQuantified::observe_bound_term() const
{
    return bound_term.get();
}

std::unique_ptr<IMutableTerm> MutableQuantified::take_bound_term()
{
    return std::move(bound_term);
}

std::unique_ptr<IMutableSentence> MutableQuantified::take_sentence()
{
    return std::move(sentence);
}

const IMutableSentence *MutableQuantified::observe_sentence() const
{
    return sentence.get();
}

void MutableQuantified::put_sentence(std::unique_ptr<IMutableSentence> &&sentence)
{
    this->sentence = std::move(sentence);
}

void MutableQuantified::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutableQuantified::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutableQuantified::hash() const noexcept
{
    auto hash = std::hash<std::underlying_type_t<QuantifierTypes>>{}(
        static_cast<std::underlying_type_t<QuantifierTypes>>(quantifier_type));

    hash = hash_combine(hash, bound_term->hash());
    hash = hash_combine(hash, sentence->hash());

    return hash_polarity(hash, is_negative_polarity());
}

std::ostream &MutableQuantified::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(' << TextSerialiserVisitor::get_operator_symbol(quantifier_type);
    bound_term->serialise(ostream);
    ostream << '(';
    sentence->serialise(ostream);
    return ostream << ')';
}

void MutableQuantified::put_bound_term(std::unique_ptr<IMutableTerm> &&new_bound_term)
{
    bound_term = std::move(new_bound_term);
}

} // namespace optifol
