/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the mutable binary-Connected Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef MUTABLEBINARYCONNECTED_HPP
#define MUTABLEBINARYCONNECTED_HPP

#include <memory>

#include "../OwningBuildable.hpp"
#include "IMutableSentence.hpp"

namespace optifol
{

enum class BinaryOperatorTypes
{
    None,
    Conjunction,
    Disjunction,
    Implication,
    Biconditional,
};

class MutableBinaryConnected :
        public IMutableSentence,
        public OwningBuildable<MutableBinaryConnected>
{
public:
    [[maybe_unused]] explicit MutableBinaryConnected(BinaryOperatorTypes operator_type,
                                                    std::unique_ptr<IMutableSentence>&& lhs,
                                                    std::unique_ptr<IMutableSentence>&& rhs,
                                                    bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentence> clone() const override;

    void flip_polarity() noexcept override;

    [[nodiscard]] bool is_negative_polarity() const noexcept override;

    [[nodiscard]] BinaryOperatorTypes get_operator_type() const;

    void set_operator_type(BinaryOperatorTypes new_type);

    [[nodiscard]] std::unique_ptr<IMutableSentence> take_lhs_operand();

    [[nodiscard]] std::unique_ptr<IMutableSentence> take_rhs_operand();

    [[nodiscard]] const IMutableSentence * observe_lhs_operand() const;

    [[nodiscard]] const IMutableSentence * observe_rhs_operand() const;

    void put_lhs_operand(std::unique_ptr<IMutableSentence>&& operand);

    void put_rhs_operand(std::unique_ptr<IMutableSentence>&& operand);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    BinaryOperatorTypes operator_type;

    std::unique_ptr<IMutableSentence> lhs;

    std::unique_ptr<IMutableSentence> rhs;

    bool is_positive;
};

}

#endif
