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

    [[nodiscard]] std::unique_ptr<ISentenceNode> get_operand();

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

private:
    std::unique_ptr<ISentenceNode> operand;
};

}

#endif
