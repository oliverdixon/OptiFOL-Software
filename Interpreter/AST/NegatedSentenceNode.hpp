//
// Created by owd on 20/11/24.
//

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
    [[maybe_unused]] explicit NegatedSentenceNode(std::shared_ptr<ISentenceNode> operand) :
            operand(std::move(operand))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return "~ " + operand->to_string();
    }

private:
    std::shared_ptr<ISentenceNode> operand;
};

}

#endif //OPTIFOL_NEGATEDSENTENCENODE_HPP
