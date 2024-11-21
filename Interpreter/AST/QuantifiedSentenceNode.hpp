//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_QUANTIFIEDSENTENCENODE_HPP
#define OPTIFOL_QUANTIFIEDSENTENCENODE_HPP

#include <memory>
#include "ISentenceNode.hpp"
#include "VariableNode.hpp"

namespace optifol
{

enum class QuantifierTypes
{
    Universal,
    Existential
};

class QuantifiedSentenceNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] QuantifiedSentenceNode(QuantifierTypes quantifierType,
                                            std::shared_ptr<VariableNode> bound_variable,
                                            std::shared_ptr<ISentenceNode> sentence) :
            quantifierType(quantifierType),
            bound_variable(std::move(bound_variable)),
            sentence(std::move(sentence))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return ((quantifierType == QuantifierTypes::Universal) ? "ForAll " : "ThereExist ") +
               bound_variable->to_string() + " (" + sentence->to_string() + ')';
    }

private:
    QuantifierTypes quantifierType;
    std::shared_ptr<VariableNode> bound_variable;
    std::shared_ptr<ISentenceNode> sentence;
};

}

#endif //OPTIFOL_QUANTIFIEDSENTENCENODE_HPP
