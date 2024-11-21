//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_CONNECTEDSENTENCENODE_HPP
#define OPTIFOL_CONNECTEDSENTENCENODE_HPP

#include "ISentenceNode.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace optifol
{

enum class BinaryOperatorTypes
{
    Conjunction,
    Disjunction,
    Implication,
    Biconditional,
};

class ConnectedSentenceNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] explicit ConnectedSentenceNode(BinaryOperatorTypes operatorType,
                                                    std::shared_ptr<ISentenceNode> lhs,
                                                    std::shared_ptr<ISentenceNode> rhs) :
            operatorType(operatorType),
            lhs(std::move(lhs)),
            rhs(std::move(rhs))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return lhs->to_string() + ' ' + operator_symbol() + ' ' + rhs->to_string();
    }

private:
    [[nodiscard]] const char *operator_symbol() const
    {
        switch (operatorType) {
            case BinaryOperatorTypes::Conjunction: return "&";
            case BinaryOperatorTypes::Disjunction: return "|";
            case BinaryOperatorTypes::Implication: return "=>";
            case BinaryOperatorTypes::Biconditional: return "<=>";
        }
    }

    BinaryOperatorTypes operatorType;
    std::shared_ptr<ISentenceNode> lhs;
    std::shared_ptr<ISentenceNode> rhs;
};

}

#endif //OPTIFOL_CONNECTEDSENTENCENODE_HPP
