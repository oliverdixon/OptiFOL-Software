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

#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"
#include "../../../Visitors/MutableTargets/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

MutableQuantified::MutableQuantified(const QuantifierTypes quantifier_type, std::unique_ptr<IMutableTerm> &&bound_term,
        std::unique_ptr<IMutableSentence> &&sentence, const bool is_positive) :
    quantifier_type(quantifier_type),
    bound_term(std::move(bound_term)),
    sentence(std::move(sentence)),
    is_positive(is_positive)
{
}

std::unique_ptr<IMutableSentence> MutableQuantified::clone() const
{
    return std::make_unique<MutableQuantified>(quantifier_type, bound_term->clone(), sentence->clone(), is_positive);
}

void MutableQuantified::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool MutableQuantified::is_negative_polarity() const noexcept
{
    return !is_positive;
}

void MutableQuantified::set_quantifier_type(const QuantifierTypes quantifier_type) noexcept
{
    this->quantifier_type = quantifier_type;
}

QuantifierTypes MutableQuantified::get_quantifier_type() const noexcept
{
    return quantifier_type;
}

const IMutableTerm *MutableQuantified::observe_bound_term() const noexcept
{
    return bound_term.get();
}

std::unique_ptr<IMutableTerm> MutableQuantified::take_bound_term() noexcept
{
    return std::move(bound_term);
}

std::unique_ptr<IMutableSentence> MutableQuantified::take_sentence() noexcept
{
    return std::move(sentence);
}

const IMutableSentence *MutableQuantified::observe_sentence() const noexcept
{
    return sentence.get();
}

void MutableQuantified::put_sentence(std::unique_ptr<IMutableSentence> &&sentence) noexcept
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

const ISentence *MutableQuantified::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

std::size_t MutableQuantified::hash() const noexcept
{
    return Quantified::hash_quantified(quantifier_type, bound_term.get(), sentence.get(), is_positive);
}

std::ostream &MutableQuantified::serialise(std::ostream &ostream) const
{
    return Quantified::serialise_quantified(ostream, quantifier_type, bound_term.get(), sentence.get(),
        is_positive);
}

void MutableQuantified::put_bound_term(std::unique_ptr<IMutableTerm> &&new_bound_term) noexcept
{
    bound_term = std::move(new_bound_term);
}

} // namespace optifol
