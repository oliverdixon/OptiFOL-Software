/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the binary-Connected Sentence IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef CONNECTEDSENTENCENODE_HPP
#define CONNECTEDSENTENCENODE_HPP

#include <memory>

#include "ISentenceNode.hpp"
#include "../Support/Buildable.hpp"

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

class ConnectedSentenceNode :
        public ISentenceNode,
        public Buildable<ConnectedSentenceNode>
{
public:
    [[maybe_unused]] explicit ConnectedSentenceNode(BinaryOperatorTypes operator_type,
                                                    std::unique_ptr<ISentenceNode>&& lhs,
                                                    std::unique_ptr<ISentenceNode>&& rhs,
                                                    bool is_positive = true);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    [[nodiscard]] BinaryOperatorTypes get_operator_type() const;

    void set_operator_type(BinaryOperatorTypes new_type);

    [[nodiscard]] std::unique_ptr<ISentenceNode> take_lhs_operand();

    [[nodiscard]] std::unique_ptr<ISentenceNode> take_rhs_operand();

    [[nodiscard]] const ISentenceNode * observe_lhs_operand() const;

    [[nodiscard]] const ISentenceNode * observe_rhs_operand() const;

    void put_lhs_operand(std::unique_ptr<ISentenceNode>&& operand);

    void put_rhs_operand(std::unique_ptr<ISentenceNode>&& operand);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

private:
    BinaryOperatorTypes operator_type;

    std::unique_ptr<ISentenceNode> lhs;

    std::unique_ptr<ISentenceNode> rhs;

    bool is_positive;
};

}

#endif
