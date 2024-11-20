//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_ITERMASTNODE_HPP
#define OPTIFOL_ITERMASTNODE_HPP

#include <string>

namespace optifol
{

class ITermASTNode
{
public:
    virtual ~ITermASTNode() = default;

    [[nodiscard]] virtual std::string to_string() const = 0;
};

}

#endif //OPTIFOL_ITERMASTNODE_HPP
