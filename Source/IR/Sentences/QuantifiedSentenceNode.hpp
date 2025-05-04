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
                                            std::unique_ptr<ITermNode>&& bound_term,
                                            std::unique_ptr<ISentenceNode>&& sentence);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    [[nodiscard]] QuantifierTypes get_quantifier_type() const;

    [[nodiscard]] const ITermNode * observe_bound_term() const;

    [[nodiscard]] std::unique_ptr<ITermNode> take_bound_term();

    [[nodiscard]] std::unique_ptr<ISentenceNode> take_sentence();

    [[nodiscard]] const ISentenceNode * observe_sentence() const;

    void put_sentence(std::unique_ptr<ISentenceNode>&& sentence);

    void swap_bound_term(std::unique_ptr<ITermNode>&& new_bound_term);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

private:
    QuantifierTypes quantifier_type;
    std::unique_ptr<ITermNode> bound_term;
    std::unique_ptr<ISentenceNode> sentence;
};

}

#endif
