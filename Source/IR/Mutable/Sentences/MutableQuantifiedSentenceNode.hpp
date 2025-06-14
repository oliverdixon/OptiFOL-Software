/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Quantified Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEQUANTIFIEDSENTENCENODE_HPP
#define MUTABLEQUANTIFIEDSENTENCENODE_HPP

#include <memory>

#include "../../Mutable/Terms/IMutableTermNode.hpp"
#include "../OwningBuildable.hpp"
#include "IMutableSentenceNode.hpp"

namespace optifol
{

enum class QuantifierTypes
{
    Universal,
    Existential
};

class MutableQuantifiedSentenceNode :
        public IMutableSentenceNode,
        public OwningBuildable<MutableQuantifiedSentenceNode>
{
public:
    [[maybe_unused]] MutableQuantifiedSentenceNode(QuantifierTypes quantifier_type,
                                            std::unique_ptr<IMutableTermNode>&& bound_term,
                                            std::unique_ptr<IMutableSentenceNode>&& sentence,
                                            bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    void set_quantifier_type(QuantifierTypes quantifier_type);

    [[nodiscard]] QuantifierTypes get_quantifier_type() const;

    [[nodiscard]] const IMutableTermNode * observe_bound_term() const;

    [[nodiscard]] std::unique_ptr<IMutableTermNode> take_bound_term();

    [[nodiscard]] std::unique_ptr<IMutableSentenceNode> take_sentence();

    [[nodiscard]] const IMutableSentenceNode * observe_sentence() const;

    void put_sentence(std::unique_ptr<IMutableSentenceNode>&& sentence);

    void put_bound_term(std::unique_ptr<IMutableTermNode>&& new_bound_term);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    QuantifierTypes quantifier_type;
    std::unique_ptr<IMutableTermNode> bound_term;
    std::unique_ptr<IMutableSentenceNode> sentence;

    bool is_positive;
};

}

#endif
