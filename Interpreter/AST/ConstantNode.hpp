//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_CONSTANTNODE_HPP
#define OPTIFOL_CONSTANTNODE_HPP

#include "ITermNode.hpp"

namespace optifol
{

class ConstantNode :
        public ITermNode
{
public:
    [[maybe_unused]] explicit ConstantNode(std::string name) :
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

#endif //OPTIFOL_CONSTANTNODE_HPP
