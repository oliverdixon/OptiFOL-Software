//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_IDENTITYTERMASTNODE_HPP
#define OPTIFOL_IDENTITYTERMASTNODE_HPP

#include "IAtomicASTNode.hpp"

#include <memory>

namespace optifol
{

class IdentityTermASTNode :
        public IAtomicASTNode
{
public:
    [[maybe_unused]] IdentityTermASTNode(std::shared_ptr<ITermASTNode> lhs,
                                         std::shared_ptr<ITermASTNode> rhs) :
            lhs(std::move(lhs)),
            rhs(std::move(rhs))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return lhs->to_string() + " %Idty " + rhs->to_string();
    }

private:
    std::shared_ptr<ITermASTNode> lhs;
    std::shared_ptr<ITermASTNode> rhs;
};

}

#endif //OPTIFOL_IDENTITYTERMASTNODE_HPP
