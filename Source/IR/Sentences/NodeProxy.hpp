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
    explicit NodeProxy(std::unique_ptr<ISentenceNode>&& node);

    [[nodiscard]] std::unique_ptr<ISentenceNode> take_sentence();

    [[nodiscard]] const ISentenceNode * observe_sentence() const;

    void put_sentence(std::unique_ptr<ISentenceNode>&& sentence);

    [[nodiscard]] std::unique_ptr<ISentenceNode> clone() const override;

    void accept(MutatingSentenceVisitorBase& visitor) override;

    void accept(IObservingSentenceVisitor& visitor) const override;

    std::unique_ptr<ISentenceNode> sentence;
};

}

#endif
