//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_PREDICATIONNODE_HPP
#define OPTIFOL_PREDICATIONNODE_HPP

#include "ISentenceNode.hpp"
#include "ITermNode.hpp"

#include <vector>
#include <memory>

namespace optifol
{

class PredicationNode :
        public ISentenceNode
{
public:
    [[maybe_unused]] explicit PredicationNode(std::string name,
                                              std::vector<std::shared_ptr<ITermNode>> &&arguments);

    [[nodiscard]] std::string to_string() const override;

    void accept(VisitorBase& visitor) override;

private:
    const std::string name;
    const std::vector<std::shared_ptr<ITermNode>> arguments;
};

}

#endif //OPTIFOL_PREDICATIONNODE_HPP
