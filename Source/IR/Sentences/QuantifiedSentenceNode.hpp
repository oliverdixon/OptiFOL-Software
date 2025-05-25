/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Quantified Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef QUANTIFIEDSENTENCENODE_HPP
#define QUANTIFIEDSENTENCENODE_HPP

#include <memory>

#include "ISentenceNode.hpp"
#include "../Terms/VariableNode.hpp"

namespace optifol
{

enum class QuantifierTypes
{
    Universal,
    Existential
};

class QuantifiedSentenceNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] QuantifiedSentenceNode(QuantifierTypes quantifier_type,
                                            std::unique_ptr<ITermNode>&& bound_term,
                                            std::unique_ptr<ISentenceNode>&& sentence,
                                            bool is_positive = true);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    void set_quantifier_type(QuantifierTypes quantifier_type);

    [[nodiscard]] QuantifierTypes get_quantifier_type() const;

    [[nodiscard]] const ITermNode * observe_bound_term() const;

    [[nodiscard]] std::unique_ptr<ITermNode> take_bound_term();

    [[nodiscard]] std::unique_ptr<ISentenceNode> take_sentence();

    [[nodiscard]] const ISentenceNode * observe_sentence() const;

    void put_sentence(std::unique_ptr<ISentenceNode>&& sentence);

    void put_bound_term(std::unique_ptr<ITermNode>&& new_bound_term);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

private:
    QuantifierTypes quantifier_type;
    std::unique_ptr<ITermNode> bound_term;
    std::unique_ptr<ISentenceNode> sentence;

    bool is_positive;
};

}

#endif
