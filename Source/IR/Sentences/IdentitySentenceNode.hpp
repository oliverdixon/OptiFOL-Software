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
    [[maybe_unused]] IdentitySentenceNode(std::shared_ptr<ITermNode> lhs,
                                          std::shared_ptr<ITermNode> rhs);

    [[nodiscard]] std::shared_ptr<ITermNode> get_lhs_operand() const;

    [[nodiscard]] std::shared_ptr<ITermNode> get_rhs_operand() const;

    void swap_lhs_operand(std::shared_ptr<ITermNode> new_lhs);

    void swap_rhs_operand(std::shared_ptr<ITermNode> new_rhs);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

private:
    std::shared_ptr<ITermNode> lhs;
    std::shared_ptr<ITermNode> rhs;
};

}

#endif //OPTIFOL_IDENTITYSENTENCENODE_HPP
