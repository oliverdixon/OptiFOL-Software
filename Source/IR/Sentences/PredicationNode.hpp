/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_PREDICATIONNODE_HPP
#define OPTIFOL_PREDICATIONNODE_HPP

#include "ISentenceNode.hpp"

#include <vector>
#include <memory>

namespace optifol
{

class ITermNode;

class PredicationNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] PredicationNode(std::string name, std::vector<std::unique_ptr<ITermNode>> &&arguments,
        bool is_positive = true);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void flip_polarity() override;

    [[nodiscard]] bool is_negative_polarity() const override;

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    const std::string name; // TODO move
    std::vector<std::unique_ptr<ITermNode>> arguments; // TODO move

private:
    bool is_positive;
};

}

#endif
