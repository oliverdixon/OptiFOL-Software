/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Quantified Sentence IR node
 * @author Oliver Dixon
 * @date 2025-06-17
 * @version Development
 */

#include <utility>

#include "../Terms/IProcessedTerm.hpp"
#include "../Terms/Variable.hpp"
#include "Quantified.hpp"

namespace optifol
{

Quantified::Quantified(const QuantifierTypes quantifier_type, const IProcessedTerm *const bound_term,
        const ISentence *const sentence, const bool is_positive) :
    quantifier_type(quantifier_type),
    bound_term(bound_term),
    sentence(sentence),
    is_positive(is_positive)
{
}

QuantifierTypes Quantified::get_quantifier_type() const noexcept
{
    return quantifier_type;
}

const IProcessedTerm *Quantified::observe_bound_term() const noexcept
{
    return bound_term;
}

const ISentence *Quantified::observe_sentence() const noexcept
{
    return sentence;
}

std::size_t Quantified::hash() const noexcept
{
    return hash_quantified(quantifier_type, bound_term, sentence, is_positive);
}

std::ostream &Quantified::serialise(std::ostream &ostream) const
{
    return serialise_quantified(ostream, quantifier_type, bound_term, sentence, is_positive);
}

bool Quantified::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::size_t Quantified::hash_quantified(const QuantifierTypes quantifier_type, const ITerm *const bound_term,
        const ISentence *const sentence, const bool is_positive)
{
    auto hash = std::hash<std::underlying_type_t<QuantifierTypes>>{}(std::to_underlying(quantifier_type));

    hash = hash_combine(hash, bound_term->hash());
    hash = hash_combine(hash, sentence->hash());

    return hash_polarity(hash, !is_positive);
}
std::ostream &Quantified::serialise_quantified(std::ostream &ostream, const QuantifierTypes quantifier_type,
        const ITerm *const bound_term, const ISentence *const sentence, const bool is_positive)
{
    if (!is_positive)
        ostream << '~';

    ostream << '(' << get_operator_symbol(quantifier_type);
    bound_term->serialise(ostream);
    ostream << '(';
    sentence->serialise(ostream);
    return ostream << ')';
}

const char *Quantified::get_operator_symbol(const QuantifierTypes type)
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
