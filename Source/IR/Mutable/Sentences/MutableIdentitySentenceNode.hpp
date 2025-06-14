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

#ifndef MUTABLEIDENTITYSENTENCENODE_HPP
#define MUTABLEIDENTITYSENTENCENODE_HPP

#include <memory>

#include "../OwningBuildable.hpp"
#include "../Terms/IMutableTermNode.hpp"
#include "IMutableSentenceNode.hpp"

namespace optifol
{

class MutableIdentitySentenceNode :
        public IMutableSentenceNode,
        public OwningBuildable<MutableIdentitySentenceNode>
{
public:
    [[maybe_unused]] MutableIdentitySentenceNode(std::unique_ptr<IMutableTermNode>&& lhs, std::unique_ptr<IMutableTermNode>&& rhs,
        bool is_positive = true);

    [[nodiscard]] std::unique_ptr<IMutableSentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    [[nodiscard]] std::unique_ptr<IMutableTermNode> take_lhs_operand();

    [[nodiscard]] std::unique_ptr<IMutableTermNode> take_rhs_operand();

    [[nodiscard]] const IMutableTermNode * observe_lhs_operand() const;

    [[nodiscard]] const IMutableTermNode * observe_rhs_operand() const;

    void swap_lhs_operand(std::unique_ptr<IMutableTermNode>&& new_lhs);

    void swap_rhs_operand(std::unique_ptr<IMutableTermNode>&& new_rhs);

    void put_lhs_operand(std::unique_ptr<IMutableTermNode>&& new_lhs);

    void put_rhs_operand(std::unique_ptr<IMutableTermNode>&& new_rhs);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    [[nodiscard]] std::size_t hash() const noexcept override;

    std::ostream &serialise(std::ostream &ostream) const override;

private:
    std::unique_ptr<IMutableTermNode> lhs;
    std::unique_ptr<IMutableTermNode> rhs;

    bool is_positive;
};

}

#endif
