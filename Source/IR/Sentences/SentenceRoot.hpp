/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-06-23
 * @version Development
 */

#ifndef SENTENCEROOT_HPP
#define SENTENCEROOT_HPP

#include "IProcessedSentence.hpp"

namespace optifol
{

/**
 * @class SentenceRoot
 * @brief A SentenceRoot denotes the root node of an immutable IR node tree. It references a single sub-sentence in a
 *  SymbolRepository that may not be mutated following construction.
 * @see MutableSentenceRoot for the mutable, owning dual
 */
class SentenceRoot : public IProcessedSentence
{
public:
    /**
     * @brief Creates a new immutable SentenceRoot with the given immediate child sentence and polarity
     * @param sentence The sentence to hold immediately under the root
     * @param is_positive Should the root node be instantiated in a positive polarity?
     */
    explicit SentenceRoot(const IProcessedSentence * sentence, bool is_positive = true);

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    std::ostream& serialise(std::ostream &ostream) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    /**
     * @brief Retrieves an observing pointer to the detained sentence
     * @return An observing pointer to the sentence
     */
    [[nodiscard]] const IProcessedSentence *observe_sentence() const noexcept;

private:
    const IProcessedSentence * const sentence;
    bool is_positive;
};

} // namespace optifol

#endif // SENTENCEROOT_HPP
