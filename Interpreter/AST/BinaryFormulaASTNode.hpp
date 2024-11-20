//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_BINARYFORMULAASTNODE_HPP
#define OPTIFOL_BINARYFORMULAASTNODE_HPP

#include "IFormulaASTNode.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace optifol
{

enum class BinaryFormulaASTTypes
{
    Conjunction,
    Disjunction,
    Implication,
    Biconditional,
};

template<BinaryFormulaASTTypes OperatorType>
class BinaryFormulaASTNode :
        public IFormulaASTNode
{
public:
    [[maybe_unused]] explicit BinaryFormulaASTNode(std::shared_ptr<IFormulaASTNode> lhs,
                                                   std::shared_ptr<IFormulaASTNode> rhs) :
            lhs(std::move(lhs)),
            rhs(std::move(rhs))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return lhs->to_string() + " %" + operator_symbol() + ' ' + rhs->to_string();
    }

private:
    static const char *operator_symbol();

    std::shared_ptr<IFormulaASTNode> lhs;
    std::shared_ptr<IFormulaASTNode> rhs;
};

}

#endif //OPTIFOL_BINARYFORMULAASTNODE_HPP
