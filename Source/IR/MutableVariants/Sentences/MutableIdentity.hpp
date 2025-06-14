/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable Identity Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEIDENTITY_HPP
#define MUTABLEIDENTITY_HPP

#include <memory>

#include "../OwningBuildable.hpp"
#include "../Terms/IMutableTerm.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

class MutableIdentity :
        public IMutableSentence,
        public OwningBuildable<MutableIdentity>
{
public:
    [[maybe_unused]] MutableIdentity(std::unique_ptr<IMutableTerm>&& lhs, std::unique_ptr<IMutableTerm>&& rhs,
        bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    [[nodiscard]] std::unique_ptr<IMutableTerm> take_lhs_operand();

    [[nodiscard]] std::unique_ptr<IMutableTerm> take_rhs_operand();

    [[nodiscard]] const IMutableTerm * observe_lhs_operand() const;

    [[nodiscard]] const IMutableTerm * observe_rhs_operand() const;

    void swap_lhs_operand(std::unique_ptr<IMutableTerm>&& new_lhs);

    void swap_rhs_operand(std::unique_ptr<IMutableTerm>&& new_rhs);

    void put_lhs_operand(std::unique_ptr<IMutableTerm>&& new_lhs);

    void put_rhs_operand(std::unique_ptr<IMutableTerm>&& new_rhs);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    std::unique_ptr<IMutableTerm> lhs;
    std::unique_ptr<IMutableTerm> rhs;

    bool is_positive;
};

}

#endif
