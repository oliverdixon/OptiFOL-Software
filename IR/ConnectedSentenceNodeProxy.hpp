//
// Created by owd on 24/11/24.
//

#ifndef OPTIFOL_CONNECTEDSENTENCENODEPROXY_HPP
#define OPTIFOL_CONNECTEDSENTENCENODEPROXY_HPP

#include <utility>

#include "ConnectedSentenceNode.hpp"

namespace optifol
{

class ConnectedSentenceNodeProxy :
        public ISentenceNode
{
public:
    explicit ConnectedSentenceNodeProxy(std::shared_ptr<ConnectedSentenceNode> node);

    [[nodiscard]] std::string to_string() const override;

    void accept(VisitorBase& visitor) override;

    std::shared_ptr<ConnectedSentenceNode> sentence;
};

}

#endif //OPTIFOL_CONNECTEDSENTENCENODEPROXY_HPP
