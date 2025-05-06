/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_IDENTITYSENTENCENODE_HPP
#define OPTIFOL_IDENTITYSENTENCENODE_HPP

#include "ISentenceNode.hpp"
#include "../Terms/ITermNode.hpp"

#include <memory>

namespace optifol
{

class IdentitySentenceNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] IdentitySentenceNode(std::unique_ptr<ITermNode>&& lhs, std::unique_ptr<ITermNode>&& rhs,
        bool is_positive = true);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    [[nodiscard]] std::unique_ptr<ITermNode> take_lhs_operand();

    [[nodiscard]] std::unique_ptr<ITermNode> take_rhs_operand();

    [[nodiscard]] const ITermNode * observe_lhs_operand() const;

    [[nodiscard]] const ITermNode * observe_rhs_operand() const;

    void swap_lhs_operand(std::unique_ptr<ITermNode>&& new_lhs);

    void swap_rhs_operand(std::unique_ptr<ITermNode>&& new_rhs);

    void put_lhs_operand(std::unique_ptr<ITermNode>&& new_lhs);

    void put_rhs_operand(std::unique_ptr<ITermNode>&& new_rhs);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

private:
    std::unique_ptr<ITermNode> lhs;
    std::unique_ptr<ITermNode> rhs;

    bool is_positive;
};

}

#endif
