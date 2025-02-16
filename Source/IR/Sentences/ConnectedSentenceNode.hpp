/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_CONNECTEDSENTENCENODE_HPP
#define OPTIFOL_CONNECTEDSENTENCENODE_HPP

#include "ISentenceNode.hpp"

#include <memory>

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
        public ISentenceNode
{
public:
    [[maybe_unused]] explicit ConnectedSentenceNode(BinaryOperatorTypes operator_type,
                                                    std::shared_ptr<ISentenceNode> lhs,
                                                    std::shared_ptr<ISentenceNode> rhs);

    [[nodiscard]] BinaryOperatorTypes get_operator_type() const;

    [[nodiscard]] std::shared_ptr<ISentenceNode> get_lhs_operand() const;

    [[nodiscard]] std::shared_ptr<ISentenceNode> get_rhs_operand() const;

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    BinaryOperatorTypes operator_type;
    std::shared_ptr<ISentenceNode> lhs;
    std::shared_ptr<ISentenceNode> rhs;
};

}

#endif
