/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "SentenceRoot.hpp"

#include "../../Visitors/Sentences/IObservingSentenceVisitor.hpp"
#include "../../Visitors/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

SentenceRoot::SentenceRoot(std::unique_ptr<ISentenceNode> &&sentence) :
    sentence(std::move(sentence))
{ }

std::unique_ptr<ISentenceNode> SentenceRoot::clone() const
{
    return std::make_unique<SentenceRoot>(sentence->clone());
}

void SentenceRoot::flip_polarity()
{
    is_positive = !is_positive;
}

bool SentenceRoot::is_negative_polarity() const
{
    return !is_positive;
}

std::unique_ptr<ISentenceNode> SentenceRoot::take_sentence()
{
    return std::move(sentence);
}

const ISentenceNode * SentenceRoot::observe_sentence() const
{
    return sentence.get();
}

void SentenceRoot::put_sentence(std::unique_ptr<ISentenceNode> &&sentence)
{
    this->sentence = std::move(sentence);
}

void SentenceRoot::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void SentenceRoot::accept(IObservingSentenceVisitor &visitor) const
{
    visitor.visit(*this);
}

std::size_t SentenceRoot::hash() const noexcept
{
    return hash_polarity(sentence->hash(), is_negative_polarity());
}

std::ostream &SentenceRoot::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(';
    sentence->serialise(ostream);
    return ostream << ')';
}

}
