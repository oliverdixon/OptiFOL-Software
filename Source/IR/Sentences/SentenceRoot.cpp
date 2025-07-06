/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-06-23
 * @version Development
 */

#include "SentenceRoot.hpp"

namespace optifol
{

SentenceRoot::SentenceRoot(const IProcessedSentence * const sentence, const bool is_positive) :
    sentence(sentence),
    is_positive(is_positive)
{
}

bool SentenceRoot::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::ostream &SentenceRoot::serialise(std::ostream &ostream) const
{
    return sentence->serialise(ostream);
}

std::size_t SentenceRoot::hash() const noexcept
{
    return sentence->hash();
}

const IProcessedSentence *SentenceRoot::observe_sentence() const noexcept
{
    return sentence;
}

} // namespace optifol
