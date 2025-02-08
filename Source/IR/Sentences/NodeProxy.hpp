/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_NODEPROXY_HPP
#define OPTIFOL_NODEPROXY_HPP

#include <memory>
#include "ISentenceNode.hpp"

namespace optifol
{

class NodeProxy :
        public ISentenceNode
{
public:
    explicit NodeProxy(std::shared_ptr<ISentenceNode> node);

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    std::shared_ptr<ISentenceNode> sentence;
};

}

#endif //OPTIFOL_NODEPROXY_HPP
