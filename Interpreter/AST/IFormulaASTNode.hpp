//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_IFORMULAASTNODE_HPP
#define OPTIFOL_IFORMULAASTNODE_HPP

#include <string>

class IFormulaASTNode
{
    virtual ~IFormulaASTNode() = default;

    [[nodiscard]] virtual std::string to_string() const = 0;
};

#endif //OPTIFOL_IFORMULAASTNODE_HPP
