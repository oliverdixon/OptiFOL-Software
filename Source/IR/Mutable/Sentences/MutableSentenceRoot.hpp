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
#include "IMutableSentenceNode.hpp"

namespace optifol
{

class MutableSentenceRoot :
        public IMutableSentenceNode,
        public OwningBuildable<MutableSentenceRoot>
{
public:
    [[maybe_unused]] explicit MutableSentenceRoot(std::unique_ptr<IMutableSentenceNode>&& sentence);

    [[nodiscard]] std::unique_ptr<IMutableSentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    [[nodiscard]] std::unique_ptr<IMutableSentenceNode> take_sentence();

    [[nodiscard]] const IMutableSentenceNode * observe_sentence() const;

    void put_sentence(std::unique_ptr<IMutableSentenceNode>&& sentence);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    std::unique_ptr<IMutableSentenceNode> sentence;

    bool is_positive = true;
};

}

#endif
