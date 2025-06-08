/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef SENTENCEROOT_HPP
#define SENTENCEROOT_HPP

#include "ISentenceNode.hpp"
#include "../Support/Buildable.hpp"

namespace optifol
{

class SentenceRoot :
        public ISentenceNode,
        public Buildable<SentenceRoot>
{
public:
    [[maybe_unused]] explicit SentenceRoot(std::unique_ptr<ISentenceNode>&& sentence);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    [[nodiscard]] std::unique_ptr<ISentenceNode> take_sentence();

    [[nodiscard]] const ISentenceNode * observe_sentence() const;

    void put_sentence(std::unique_ptr<ISentenceNode>&& sentence);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

private:
    std::unique_ptr<ISentenceNode> sentence;

    bool is_positive = true;
};

}

#endif
