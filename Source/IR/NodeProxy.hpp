//
// Created by owd on 24/11/24.
//

#ifndef OPTIFOL_NODEPROXY_HPP
#define OPTIFOL_NODEPROXY_HPP

#include <utility>

#include "QuantifiedSentenceNode.hpp"

namespace optifol
{

class NodeProxy :
        public ISentenceNode
{
public:
    explicit NodeProxy(std::shared_ptr<ISentenceNode> node);

    [[nodiscard]] std::string to_string() const override;

    void accept(VisitorBase& visitor) override;

    std::shared_ptr<ISentenceNode> sentence;
};

}

#endif //OPTIFOL_NODEPROXY_HPP
