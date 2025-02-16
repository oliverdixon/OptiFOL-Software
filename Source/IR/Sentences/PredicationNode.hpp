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
    [[maybe_unused]] explicit PredicationNode(std::string name,
                                              std::vector<std::shared_ptr<ITermNode>> &&arguments);

    void accept(MutatingSentenceVisitorBase &visitor) override;

    void accept(IObservingSentenceVisitor &visitor) const override;

    const std::string name;
    std::vector<std::shared_ptr<ITermNode>> arguments;
};

}

#endif
