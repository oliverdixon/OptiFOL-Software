//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_VARIABLEASTNODE_HPP
#define OPTIFOL_VARIABLEASTNODE_HPP

#include "ITermASTNode.hpp"

class VariableASTNode:
        public ITermASTNode
{
public:
    [[maybe_unused]] explicit VariableASTNode(std::string name):
        name(std::move(name)) { }

    [[nodiscard]] std::string to_string() const override
    {
        return name;
    }

private:
    const std::string name;
};

#endif //OPTIFOL_VARIABLEASTNODE_HPP
