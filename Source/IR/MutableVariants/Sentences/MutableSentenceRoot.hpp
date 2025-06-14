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

class MutableSentenceRoot :
        public IMutableSentence,
        public OwningBuildable<MutableSentenceRoot>
{
public:
    [[maybe_unused]] explicit MutableSentenceRoot(std::unique_ptr<IMutableSentence>&& sentence);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    [[nodiscard]] std::unique_ptr<IMutableSentence> take_sentence();

    [[nodiscard]] const IMutableSentence * observe_sentence() const;

    void put_sentence(std::unique_ptr<IMutableSentence>&& sentence);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    std::unique_ptr<IMutableSentence> sentence;

    bool is_positive = true;
};

}

#endif
