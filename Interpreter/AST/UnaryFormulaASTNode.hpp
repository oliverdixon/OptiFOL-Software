//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_UNARYFORMULAASTNODE_HPP
#define OPTIFOL_UNARYFORMULAASTNODE_HPP

#include "IFormulaASTNode.hpp"

#include <memory>

namespace optifol
{

enum class UnaryFormulaASTTypes
{
    Negation
};

template<UnaryFormulaASTTypes OperatorType>
class UnaryFormulaASTNode :
        public IFormulaASTNode
{
public:
    [[maybe_unused]] explicit UnaryFormulaASTNode(std::shared_ptr<IFormulaASTNode> operand) :
            operand(std::move(operand))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return std::string("%") + operator_symbol() + ' ' + operand->to_string();
    }

private:
    static const char *operator_symbol();

    std::shared_ptr<IFormulaASTNode> operand;
};

}

#endif //OPTIFOL_UNARYFORMULAASTNODE_HPP
