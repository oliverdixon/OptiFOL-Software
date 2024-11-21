//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_ITERMNODE_HPP
#define OPTIFOL_ITERMNODE_HPP

#include <string>

namespace optifol
{

class ITermNode
{
public:
    virtual ~ITermNode() = default;

    [[nodiscard]] virtual std::string to_string() const = 0;
};

}

#endif //OPTIFOL_ITERMNODE_HPP
