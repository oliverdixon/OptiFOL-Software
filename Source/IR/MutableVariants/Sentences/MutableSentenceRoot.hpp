/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLESENTENCEROOT_HPP
#define MUTABLESENTENCEROOT_HPP

#include "../OwningBuildable.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

/**
 * @class MutableSentenceRoot
 * @brief A MutableSentenceRoot denotes the root node of a mutable IR node tree. It owns a single sub-sentence that may
 *  be mutated and transferred.
 * @see SentenceRoot for the immutable, non-owning dual
 */
class MutableSentenceRoot : public IMutableSentence,
                            public OwningBuildable<MutableSentenceRoot>
{
public:
    /**
     * @brief Construct a new SentenceRoot to encapsulate and own the given sub-sentence
     * @param sentence The container of the sentence whose ownership is to be transferred into the SentenceRoot
     */
    [[maybe_unused]] explicit MutableSentenceRoot(std::unique_ptr<IMutableSentence> &&sentence);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    /**
     * @copydetails IMutableSentence::accept(RepositoryBuildingVisitor&)
     * @return A @ref std::nullptr_t value, as roots are not managed by any SymbolRepository.
     */
    const ISentence *accept(RepositoryBuildingVisitor &visitor) override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

    /**
     * @brief Steals ownership of the contained sentence
     * @return The stolen container of the sentence
     * @warning This call transfers ownership outbound
     */
    [[nodiscard]] std::unique_ptr<IMutableSentence> take_sentence() noexcept;

    /**
     * @brief Retrieves a pointer to the immutable owned sentence
     * @return An immutable pointer to the owned sentence
     */
    [[nodiscard]] const IMutableSentence *observe_sentence() const noexcept;

    /**
     * @brief Transfers ownership of a new sentence, overwriting any previously held sentence
     * @param sentence The new sentence-owning container
     * @warning This call transfers ownership inbound
     * @see @ref std::unique_ptr::operator= for semantics of swap
     */
    void put_sentence(std::unique_ptr<IMutableSentence> &&sentence) noexcept;

private:
    std::unique_ptr<IMutableSentence> sentence;
    bool is_positive = true;
};

} // namespace optifol

#endif
