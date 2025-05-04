/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_NEGATEDSENTENCENODE_HPP
#define OPTIFOL_NEGATEDSENTENCENODE_HPP

#include "ISentenceNode.hpp"

#include <memory>

namespace optifol
{

class NegatedSentenceNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] explicit NegatedSentenceNode(std::unique_ptr<ISentenceNode>&& operand);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    [[nodiscard]] std::unique_ptr<ISentenceNode> take_operand();

    [[nodiscard]] const ISentenceNode * observe_operand() const;

    void put_operand(std::unique_ptr<ISentenceNode>&& operand);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

private:
    std::unique_ptr<ISentenceNode> operand;
};

}

#endif
