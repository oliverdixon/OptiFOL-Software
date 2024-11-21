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
                                              std::vector<std::shared_ptr<ITermNode>> &&arguments) :
            name(std::move(name)),
            arguments(std::move(arguments))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        std::string result = name + '(';

        auto argument_count = arguments.size();

        for (const auto &arg: arguments) {
            result += arg->to_string();
            if (--argument_count > 0)
                result += ", ";
        }

        result += ')';
        return result;
    }

private:
    const std::string name;
    const std::vector<std::shared_ptr<ITermNode>> arguments;
};

}

#endif //OPTIFOL_PREDICATIONNODE_HPP
