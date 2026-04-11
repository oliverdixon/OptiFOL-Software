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

#include <utility>

#include "../../../Visitors/MutableTargets/Observers/IObservingNodeVisitor.hpp"
#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

MutableQuantified::MutableQuantified(const QuantifierTypes quantifier_type,
        std::unique_ptr<IMutableTerm> &&bound_term, std::unique_ptr<IMutableSentence> &&sentence,
        const bool is_positive) :
    quantifier_type(quantifier_type),
    bound_term(std::move(bound_term)),
    sentence(std::move(sentence)),
    is_positive(is_positive)
{
}

std::unique_ptr<IMutableSentence> MutableQuantified::clone() const
{
    return std::make_unique<MutableQuantified>(
            quantifier_type, bound_term->clone(), sentence->clone(), is_positive);
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

void MutableQuantified::accept(IObservingNodeVisitor &visitor) const
{
    visitor.visit(*this);
}

const IProcessedSentence *MutableQuantified::accept(RepositoryBuildingVisitor &visitor)
{
    return visitor.visit(*this);
}

std::size_t MutableQuantified::hash() const noexcept
{
    auto hash = std::hash<std::underlying_type_t<QuantifierTypes>>{}(std::to_underlying(quantifier_type));

    hash = hash_combine(hash, bound_term->hash());
    hash = hash_combine(hash, sentence->hash());

    return hash_polarity(hash, !is_positive);
}

std::ostream &MutableQuantified::serialise(std::ostream &ostream) const
{
    if (!is_positive)
        ostream << '~';

    ostream << '(' << get_operator_symbol(quantifier_type);
    bound_term->serialise(ostream);
    ostream << '(';
    sentence->serialise(ostream);
    return ostream << ')';
}

bool MutableQuantified::operator==(const IMutableSentence &other) const noexcept
{
    const auto other_quantified = dynamic_cast<const MutableQuantified *>(&other);
    if (other_quantified == nullptr)
        // Other IProcessedSentence isn't a MutableQuantified.
        return false;

    if (is_positive != other_quantified->is_positive)
        // Different signs.
        return false;

    if (quantifier_type != other_quantified->quantifier_type)
        // Different quantifier nature.
        return false;

    bool bound_terms_match = false;

    if (bound_term == nullptr)
        bound_terms_match = other_quantified->bound_term == nullptr;
    else if (other_quantified->bound_term == nullptr)
        bound_terms_match = bound_term == nullptr;
    else
        // Both bound terms are non-NULL and should be compared with IMutableTerm::operator==.
        bound_terms_match = *bound_term == *other_quantified->bound_term;

    if (!bound_terms_match)
        return false;

    /*
     * Bound terms and quantifier natures match. Test the sentences for NULL, otherwise use
     * IMutableSentence::operator==.
     */
    if (sentence == nullptr || other_quantified->sentence == nullptr)
        return sentence == other_quantified->sentence;

    return *sentence == *other_quantified->sentence;
}

void MutableQuantified::put_bound_term(std::unique_ptr<IMutableTerm> &&new_bound_term) noexcept
{
    bound_term = std::move(new_bound_term);
}

const char *MutableQuantified::get_operator_symbol(const QuantifierTypes type)
{
    switch (type) {
    case QuantifierTypes::Universal:
        return "ForAll ";
    case QuantifierTypes::Existential:
        return "ThereExists ";
    }

    return " ??? ";
}

} // namespace optifol
