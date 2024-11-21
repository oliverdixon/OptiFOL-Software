//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_IDENTITYSENTENCENODE_HPP
#define OPTIFOL_IDENTITYSENTENCENODE_HPP

#include "ISentenceNode.hpp"

#include <memory>

namespace optifol
{

class IdentitySentenceNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] IdentitySentenceNode(std::shared_ptr<ITermNode> lhs,
                                          std::shared_ptr<ITermNode> rhs) :
            lhs(std::move(lhs)),
            rhs(std::move(rhs))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return lhs->to_string() + " = " + rhs->to_string();
    }

private:
    std::shared_ptr<ITermNode> lhs;
    std::shared_ptr<ITermNode> rhs;
};

}

#endif //OPTIFOL_IDENTITYSENTENCENODE_HPP
