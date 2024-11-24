//
// Created by owd on 24/11/24.
//

#ifndef OPTIFOL_QUANTIFIEDSENTENCENODEPROXY_HPP
#define OPTIFOL_QUANTIFIEDSENTENCENODEPROXY_HPP

#include <utility>

#include "QuantifiedSentenceNode.hpp"

namespace optifol
{

class QuantifiedSentenceNodeProxy :
        public ISentenceNode
{
public:
    explicit QuantifiedSentenceNodeProxy(std::shared_ptr<QuantifiedSentenceNode> node);

    [[nodiscard]] std::string to_string() const override;

    void accept(VisitorBase& visitor) override;

    std::shared_ptr<QuantifiedSentenceNode> sentence;
};

}

#endif //OPTIFOL_QUANTIFIEDSENTENCENODEPROXY_HPP
