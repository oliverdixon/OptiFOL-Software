//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_FUNCTIONASTNODE_HPP
#define OPTIFOL_FUNCTIONASTNODE_HPP

#include <memory>
#include <utility>
#include <vector>
#include "ITermASTNode.hpp"

class FunctionASTNode :
        public ITermASTNode
{
public:
    [[maybe_unused]] explicit FunctionASTNode(std::string name,
                                              std::vector<std::shared_ptr<ITermASTNode>> &&arguments) :
            name(std::move(name)), arguments(std::move(arguments))
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

#endif //OPTIFOL_FUNCTIONASTNODE_HPP
