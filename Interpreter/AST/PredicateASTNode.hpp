//
// Created by owd on 20/11/24.
//

#ifndef OPTIFOL_PREDICATEASTNODE_HPP
#define OPTIFOL_PREDICATEASTNODE_HPP

#include "IAtomicASTNode.hpp"
#include "ITermASTNode.hpp"

#include <vector>
#include <memory>

namespace optifol
{

class PredicateASTNode :
        public IAtomicASTNode
{
public:
    [[maybe_unused]] explicit PredicateASTNode(std::string name,
                                               std::vector<std::shared_ptr<ITermASTNode>> &&arguments) :
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
    const std::vector<std::shared_ptr<ITermASTNode>> arguments;
};

}

#endif //OPTIFOL_PREDICATEASTNODE_HPP
