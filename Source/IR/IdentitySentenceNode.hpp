//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_IDENTITYSENTENCENODE_HPP
#define OPTIFOL_IDENTITYSENTENCENODE_HPP

#include "ISentenceNode.hpp"
#include "ITermNode.hpp"

#include <memory>

namespace optifol
{

class IdentitySentenceNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] IdentitySentenceNode(std::shared_ptr<ITermNode> lhs,
                                          std::shared_ptr<ITermNode> rhs);

    [[nodiscard]] std::shared_ptr<ITermNode> get_lhs_operand() const;

    [[nodiscard]] std::shared_ptr<ITermNode> get_rhs_operand() const;

    void accept(MutatingVisitorBase& visitor) override;

    void accept(IObservingVisitor& visitor) const override;

private:
    std::shared_ptr<ITermNode> lhs;
    std::shared_ptr<ITermNode> rhs;
};

}

#endif //OPTIFOL_IDENTITYSENTENCENODE_HPP
