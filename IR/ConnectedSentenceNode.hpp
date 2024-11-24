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
    [[maybe_unused]] explicit ConnectedSentenceNode(BinaryOperatorTypes operator_type,
                                                    std::shared_ptr<ISentenceNode> lhs,
                                                    std::shared_ptr<ISentenceNode> rhs);

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] BinaryOperatorTypes get_operator_type() const;

    [[nodiscard]] std::shared_ptr<ISentenceNode> get_lhs_operand() const;

    [[nodiscard]] std::shared_ptr<ISentenceNode> get_rhs_operand() const;

    void accept(VisitorBase& visitor) override;

    BinaryOperatorTypes operator_type;
    std::shared_ptr<ISentenceNode> lhs;
    std::shared_ptr<ISentenceNode> rhs;

private:
    [[nodiscard]] const char *operator_symbol() const
    {
        switch (operator_type) {
            case BinaryOperatorTypes::Conjunction:
                return "&";
            case BinaryOperatorTypes::Disjunction:
                return "|";
            case BinaryOperatorTypes::Implication:
                return "=>";
            case BinaryOperatorTypes::Biconditional:
                return "<=>";
        }
    }
};

}

#endif //OPTIFOL_CONNECTEDSENTENCENODE_HPP
