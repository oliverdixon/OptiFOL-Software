//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_CONSTANTASTNODE_HPP
#define OPTIFOL_CONSTANTASTNODE_HPP

#include "ITermASTNode.hpp"

namespace optifol
{

class ConstantASTNode :
        public ITermASTNode
{
public:
    [[maybe_unused]] explicit ConstantASTNode(std::string name) :
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

#endif //OPTIFOL_CONSTANTASTNODE_HPP
