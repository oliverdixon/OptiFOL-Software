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
    [[maybe_unused]] QuantifiedSentenceNode(QuantifierTypes quantifier_type,
                                            std::shared_ptr<VariableNode> bound_variable,
                                            std::shared_ptr<ISentenceNode> sentence);

    [[nodiscard]] QuantifierTypes get_quantifier_type() const;

    [[nodiscard]] std::shared_ptr<VariableNode> get_bound_variable() const;

    [[nodiscard]] std::shared_ptr<ISentenceNode> get_sentence() const;

    [[nodiscard]] std::shared_ptr<ISentenceNode> move_sentence();

    void replace_bound_variable(std::shared_ptr<VariableNode> new_bound_variable);

    void accept(MutatingVisitorBase& visitor) override;

    void accept(IObservingVisitor& visitor) const override;

private:
    QuantifierTypes quantifier_type;
    std::shared_ptr<VariableNode> bound_variable;
    std::shared_ptr<ISentenceNode> sentence;
};

}

#endif //OPTIFOL_QUANTIFIEDSENTENCENODE_HPP
