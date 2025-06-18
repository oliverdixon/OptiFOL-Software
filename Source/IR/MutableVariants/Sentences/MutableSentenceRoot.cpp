/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableSentenceRoot.hpp"

#include "../../../Visitors/MutableTargets/Sentences/IObservingSentenceVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

MutableSentenceRoot::MutableSentenceRoot(std::unique_ptr<IMutableSentence> &&sentence) :
    sentence(std::move(sentence))
{
}

std::unique_ptr<IMutableSentence> MutableSentenceRoot::clone() const
{
    return std::make_unique<MutableSentenceRoot>(sentence->clone());
}

void MutableSentenceRoot::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool MutableSentenceRoot::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::unique_ptr<IMutableSentence> MutableSentenceRoot::take_sentence()
{
    return std::move(sentence);
}

const IMutableSentence *MutableSentenceRoot::observe_sentence() const
{
    return sentence.get();
}

void MutableSentenceRoot::put_sentence(std::unique_ptr<IMutableSentence> &&sentence)
{
    this->sentence = std::move(sentence);
}

void MutableSentenceRoot::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutableSentenceRoot::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t MutableSentenceRoot::hash() const noexcept
{
    return hash_polarity(sentence->hash(), is_negative_polarity());
}

std::ostream &MutableSentenceRoot::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(';
    sentence->serialise(ostream);
    return ostream << ')';
}

} // namespace optifol
