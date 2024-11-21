//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_VARIABLENODE_HPP
#define OPTIFOL_VARIABLENODE_HPP

#include "ITermNode.hpp"

namespace optifol
{

class VariableNode :
        public ITermNode
{
public:
    [[maybe_unused]] explicit VariableNode(std::string name) :
            name(std::move(name))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return name;
    }

private:
    const std::string name;
};

}

#endif //OPTIFOL_VARIABLENODE_HPP
