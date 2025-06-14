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

#ifndef MUTABLEQUANTIFIED_HPP
#define MUTABLEQUANTIFIED_HPP

#include <memory>

#include "../../MutableVariants/Terms/IMutableTerm.hpp"
#include "../OwningBuildable.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

enum class QuantifierTypes
{
    Universal,
    Existential
};

class MutableQuantified :
        public IMutableSentence,
        public OwningBuildable<MutableQuantified>
{
public:
    [[maybe_unused]] MutableQuantified(QuantifierTypes quantifier_type,
                                            std::unique_ptr<IMutableTerm>&& bound_term,
                                            std::unique_ptr<IMutableSentence>&& sentence,
                                            bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    void set_quantifier_type(QuantifierTypes quantifier_type);

    [[nodiscard]] QuantifierTypes get_quantifier_type() const;

    [[nodiscard]] const IMutableTerm * observe_bound_term() const;

    [[nodiscard]] std::unique_ptr<IMutableTerm> take_bound_term();

    [[nodiscard]] std::unique_ptr<IMutableSentence> take_sentence();

    [[nodiscard]] const IMutableSentence * observe_sentence() const;

    void put_sentence(std::unique_ptr<IMutableSentence>&& sentence);

    void put_bound_term(std::unique_ptr<IMutableTerm>&& new_bound_term);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    QuantifierTypes quantifier_type;
    std::unique_ptr<IMutableTerm> bound_term;
    std::unique_ptr<IMutableSentence> sentence;

    bool is_positive;
};

}

#endif
