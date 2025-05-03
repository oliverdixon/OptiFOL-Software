/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_QUANTIFIEDSENTENCENODE_HPP
#define OPTIFOL_QUANTIFIEDSENTENCENODE_HPP

#include <memory>

#include "ISentenceNode.hpp"
#include "../Terms/VariableNode.hpp"

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
    [[maybe_unused]] QuantifiedSentenceNode(QuantifierTypes quantifier_type,
                                            std::unique_ptr<VariableNode>&& bound_variable,
                                            std::unique_ptr<ISentenceNode>&& sentence);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    [[nodiscard]] QuantifierTypes get_quantifier_type() const;

    [[nodiscard]] std::unique_ptr<VariableNode> get_bound_variable();

    [[nodiscard]] std::unique_ptr<ISentenceNode> get_sentence();

    [[nodiscard]] std::unique_ptr<ISentenceNode> move_sentence();

    void swap_bound_variable(std::unique_ptr<VariableNode>&& new_bound_variable);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

private:
    QuantifierTypes quantifier_type;
    std::unique_ptr<VariableNode> bound_variable;
    std::unique_ptr<ISentenceNode> sentence;
};

}

#endif
