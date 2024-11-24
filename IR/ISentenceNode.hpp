//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_ISENTENCENODE_HPP
#define OPTIFOL_ISENTENCENODE_HPP

#include <string>

namespace optifol
{

class VisitorBase;

class ISentenceNode
{
public:
    virtual ~ISentenceNode() = default;

    [[nodiscard]] virtual std::string to_string() const = 0;

    virtual void accept(VisitorBase &visitor) = 0;
};

}

#endif //OPTIFOL_ISENTENCENODE_HPP
